/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) John Pankov 2008 <pankov@adsl.by>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <glade/glade.h>

#include "callbacks.h"


#define GLADE_FILE "/usr/share/gvpn/gvpn.glade"
#define ICON_FILE PACKAGE_DATA_DIR"/gvpn/icon.png"
#define GLADE_FILE1 "gvpn.glade"


GladeXML *gxml;
GtkWidget *window;
GtkWidget* wndSettings;
GdkWindow* graph;

char RDir[256];


char IPL[256],IPR[256],IPG[256],SSent[256],SRcvd[256];
int Connected=-1; //-1 = init 0=disconn 1=conn 2=connecting
char constext[32768]="";
int fatal=1;
const char *authors[] =
{
	"John Pankov <pankov@adsl.by>",
	NULL
};

#include "comm.h"
#include "cfg.h"

extern int CommClientSocket;
extern int CommClientStatus;
extern char CFILE[256],CVPNGW[256],CLogin[256],CPW[256],CSGW[256],CDV[256];


static void ConsoleAdd(char* str){ //Add a line to ConsoleAdd
	GtkWidget* txt=glade_xml_get_widget(gxml, "tv");
	GtkTextBuffer* tb;
	GtkTextIter ti;
	
	tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt));
	strcat(constext,str);
	gtk_text_buffer_set_text (tb, constext, -1);
	gtk_text_buffer_get_end_iter(tb, &ti);
	
	gtk_widget_set_size_request(window, 360,240);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW (txt), &ti, 0, 0, 0, 0);
	gtk_main_iteration_do(0);
	
	gtk_text_buffer_get_end_iter(tb, &ti);
	
	gtk_widget_set_size_request(window, 360,240);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW (txt), &ti, 0, 0, 0, 0);
	
}
static void ErrorBox_Signal (GtkWidget* wnd) {
	gtk_widget_hide(wnd);
	if (fatal==1) exit(1);
	fatal=-1;
}


void ErrorBox(char* err, int ftl) { //Shows error messagebox. ftl=1 will cause exit(1)
	GtkWidget* gw=gtk_message_dialog_new(GTK_WINDOW(window),1,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,err);
	gtk_widget_show(gw);
	fatal=ftl;
	gtk_signal_connect (GTK_OBJECT (gw), "response", GTK_SIGNAL_FUNC (ErrorBox_Signal),
						NULL);
	while (fatal!=-1) {
		gtk_main_iteration();
	}
}


void GetFile(char* buf, char* file) {
	strcpy(buf,RDir);
	strcat(buf,file);
}


#include "notify.c"
#include "ui.c"
#include "handlers.c"

void Process(char* cmd) { //Process a message from daemon
	char out[256];
	strcpy(out,cmd);
	printf("Received: %s\n",cmd);
	if (out[0]=='d') {
		strcpy(out,cmd+1);
		strcat(out,"\n");
		ConsoleAdd(out);
	}
	if (out[0]=='m') {
		strcpy(out,cmd+1);
		strcat(out,"\n");
		ConsoleAdd(out);
	}
	if (out[0]=='e') {
		strcpy(out,cmd+1);
		strcat(out,"\n");
		ConsoleAdd("Error:\n");
		int ec=0;
		switch (out[0]) {
			case 'i':
				ec=0;break; //Uknown error
			case 'c':
				ec=1;break; //Connection error
			case 'a':
				ec=2;break; //Authorization error
		}
		ConsoleAdd (Errors[ec]);
		ErrorBox (Errors[ec], FALSE);
	}
	if (out[0]=='o') {
		if (out[1]=='d'){
			strcpy(out,cmd+2);
			gtk_label_set_text(devlabel, out);
		}
		if (out[1]=='l') strcpy(IPL,cmd+2);
		if (out[1]=='r') strcpy(IPR,cmd+2);
		if (out[1]=='g') strcpy(IPG,cmd+2);
		if (out[1]=='s') strcpy(SSent,cmd+2);
		if (out[1]=='c') strcpy(SRcvd,cmd+2);
	}
	if (out[0]=='s') {
		strcpy(out,cmd+1);
		gtk_label_set_text(actlabel, out);
	}
	if (out[0]=='p') {
		if (out[1]=='0') {
			gtk_progress_set_value (pbr, 0.0);
			Connected=0;
		}
		if (out[1]=='a') {
			gtk_progress_set_value (pbr, 33.33);
			Connected=2;
			//gtk_label_set_text(stlabel,"Connecting...");
		}
		if (out[1]=='b') gtk_progress_set_value (pbr, 66.66);
		if (out[1]=='c') {
			gtk_progress_set_value (pbr, 100.0);
			//gtk_label_set_text(stlabel,"Connected");
			Connected=1;
		}
	}
}

int Check(){ //Check for messages from daemon & update labels
	if (Connected==-1) return 1;
	if (CommClientStatus<2) { //Lost connection
		ErrorBox("Lost connection to daemon", 1);
	}
	
	char str[256];
	struct pollfd pfd;
	pfd.fd=CommClientSocket;
	pfd.events=POLLIN;
	poll(&pfd,1,1);
	if ((pfd.revents & POLLIN) != 0) {
		CommClientReceiveString (str);
		Process(str);
	}
	
	NotifyUpdate();
	
	if (!GTK_WIDGET_VISIBLE(GTK_WIDGET(window))) return 1;
	
	UpdateDataLabel();
	SetButtons();
	
	return 1;
}

int Stat() {//Send stat requests
	if (Connected==1) CommClientSendString ("stat");
	return 1;
}


int Connect() {//Initial connect to daemon
	if (CommClientInit("/tmp/gvpn-daemon")==0) {
		ErrorBox("Cannot create socket. Check your account permissions", 1);
	}
	if (CommClientConnect()==0) {
		ErrorBox("Cannot connect to gvpn-daemon. Check if it is running.\nTry running /etc/init.d/gvpn-daemon start", 1);	
	}
	ConsoleAdd("Connected to daemon\n");
	char s[256];
	CommClientReceiveString (s);
	Connected=(strcmp(s,"sc")==0)?1:0;
	
	gtk_widget_show (window);
	gtk_timeout_add (100, (GtkFunction)Check, NULL); //Run the connection manager 
	
	return 0;
}

int main (int argc, char *argv[]) {		
	char buf[256];
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	CfgLoad();
	
	gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
						argc, argv,
						GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
						NULL);
	
	//Load Glade XML and by the way, resolve installation directory
	if ((gxml = glade_xml_new (GLADE_FILE1, NULL, NULL))==NULL) {
		if ((gxml = glade_xml_new (GLADE_FILE, NULL, NULL))==NULL) {
			printf("Cannot load Glade XML file\n");
			return 1;
		} else {
			strcpy(RDir,"/usr/share/gvpn");
		}
	} else {
		getcwd (RDir,256);
	}
	
	NotifyInit();
	
	glade_xml_signal_autoconnect (gxml);
	window = glade_xml_get_widget (gxml, "wndMain");
	wndSettings = glade_xml_get_widget (gxml, "dlgSettings");
	
	GetFile (buf, "/icon.png");
	gtk_window_set_icon_from_file (window,buf,NULL);
	
	gtk_timeout_add (300, (GtkFunction)Connect, NULL); //Run connect AFTER gtk_main started!
	gtk_timeout_add (2000, (GtkFunction)Stat, NULL);
	
	UIInit();
	
	gtk_main ();
	return 0;
}


