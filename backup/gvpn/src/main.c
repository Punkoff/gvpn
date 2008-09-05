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
#include <sys/poll.h>

 #define GLADE_FILE PACKAGE_DATA_DIR"/gvpn/glade/gvpn.glade"
 #define ICON_FILE PACKAGE_DATA_DIR"/gvpn/icon.png"
//#define GLADE_FILE "gvpn.glade"


GladeXML *gxml;
GtkWidget *window;
GtkLabel *stlabel;
GtkLabel *actlabel;
GtkLabel *dtlabel;
GtkLabel *devlabel;
GtkProgress *pbr;
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
#include "callbacks.h"

extern int CommClientSocket;
extern int CommClientStatus;

static void console(char* str){ //Add a line to console
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

static void ErrorBox_Signal (GtkWidget* wnd)
{
	gtk_widget_hide(wnd);
	if (fatal==1) exit(1);
	fatal=-1;
}

void ErrorBox(char* err, int ftl) { //Show error messagebox. ftl=1 will cause exit(1)
	GtkWidget* gw=gtk_message_dialog_new(GTK_WINDOW(window),1,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,err);
 	gtk_widget_show(gw);
	fatal=ftl;
	gtk_signal_connect (GTK_OBJECT (gw), "response", GTK_SIGNAL_FUNC (ErrorBox_Signal),
		NULL);
	while (fatal!=-1) {
		gtk_main_iteration();
	}
}

void on_tbAbout_clicked() {
	gtk_show_about_dialog(window,
						  "authors", authors,
						  "program-name","GVPN",
						  "comments","PPTP client",
						  "logo", gtk_window_get_icon(window),
//						  "license","GNU Public License",
											   NULL);
}

void on_tbConnect_clicked() { //Connect 
	console("----------------------\n");
	console("Starting connection...\n");
	
	CommClientSendString("setpptpgw");
	CommClientSendString("81.25.32.68");
	CommClientSendString("setlogin");
	CommClientSendString("V2028PANKOV");
	CommClientSendString("setpassword");
	CommClientSendString("25011993");
	CommClientSendString("setdev");
	CommClientSendString("eth0");
	CommClientSendString("setlangw");
	CommClientSendString("10.10.10.2");
	CommClientSendString("connect");
}

void on_tbDisconnect_clicked() { //Disconnect
	CommClientSendString ("abort");
	gtk_progress_set_value (pbr, 0.0);
	gtk_label_set_text(stlabel,"Disconnected");
	gtk_label_set_text(devlabel,"");
	gtk_label_set_text(actlabel,"");
	gtk_label_set_text(dtlabel,"Not connected");
	Connected=0;
	console("Disconnected\n");
}

void UpdateDataLabel(){
	char s[256];
	if (Connected==1){
		strcpy(s,"Gateway IP: ");
		strcat(s,IPG);
		strcat(s,"\nLocal IP: ");
		strcat(s,IPL);
		strcat(s,"\nRemote IP: ");
		strcat(s,IPR);
		strcat(s,"\nData received: ");
		strcat(s,SRcvd);
		strcat(s,"\nData sent: ");
		strcat(s,SSent);
		gtk_label_set_text(dtlabel,s);
		gtk_label_set_text(stlabel,"Connected");
		gtk_label_set_text(actlabel,"Idle");
	}
	if (Connected==0) {
		gtk_label_set_text(stlabel,"Disconnected");
		gtk_label_set_text(dtlabel,"No info available");
		gtk_label_set_text(actlabel,"Idle");
	}
	if (Connected==2) gtk_label_set_text(stlabel,"Connecting...");
	
}

void Process(char* cmd) { //Process a message from daemon
	char out[256];
	strcpy(out,cmd);
	printf("Received: %s\n",cmd);
	if (out[0]=='d') {
		strcpy(out,cmd+1);
		strcat(out,"\n");
		console(out);
	}
	if (out[0]=='m') {
		strcpy(out,cmd+1);
		strcat(out,"\n");
		console(out);
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
	if (CommClientStatus<2) {
		ErrorBox("Lost connection to daemon", 1);
	}
	char str[256];
//	gtk_main_iteration_do(0);
	struct pollfd pfd;
	pfd.fd=CommClientSocket;
	pfd.events=POLLIN;
	poll(&pfd,1,1);
	if ((pfd.revents & POLLIN) != 0) {
		CommClientReceiveString (str);
		Process(str);
	}
	UpdateDataLabel();
	return 1;
}

int Stat() {
	if (Connected==1) CommClientSendString ("stat");
	return 1;
}

int Connect() {
	if (CommClientInit("/home/john/gvpn-daemon")==0) {
		ErrorBox("Cannot create socket", 1);
	}
	if (CommClientConnect()==0) {
		ErrorBox("Cannot connect to gvpn-daemon. Check if it is running.", 1);	
	}

	char s[256];
	CommClientReceiveString (s);
	Connected=(strcmp(s,"sc")==0)?1:0;

	gtk_widget_show (window);
	gtk_timeout_add (100, (GtkFunction)Check, NULL); //Run the connection manager 
	
	return 0;
}

int main (int argc, char *argv[])
{		
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
                      argc, argv,
                      GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
                      NULL);


	
	gxml = glade_xml_new (GLADE_FILE, NULL, NULL);
	glade_xml_signal_autoconnect (gxml);
	window = glade_xml_get_widget (gxml, "wndMain");
	
	
	gtk_timeout_add (100, (GtkFunction)Connect, NULL);
	gtk_timeout_add (2000, (GtkFunction)Stat, NULL);
	
	//Set styles
	stlabel=glade_xml_get_widget (gxml, "stlabel");
	dtlabel=glade_xml_get_widget (gxml, "dtlabel");
	devlabel=glade_xml_get_widget (gxml, "devlabel");
	pbr=glade_xml_get_widget (gxml, "pbr");
	actlabel=glade_xml_get_widget (gxml, "actlabel");
	
	PangoFontDescription *font_desc = pango_font_description_from_string ("sans serif 16");
	gtk_widget_modify_font (stlabel, font_desc);
	pango_font_description_free (font_desc);
	font_desc = pango_font_description_from_string ("sans serif bold 8");
	gtk_widget_modify_font (devlabel, font_desc);
	pango_font_description_free (font_desc);
	
	gtk_window_set_icon_from_file(window, "icon.png", NULL);
	
	gtk_misc_set_alignment(GTK_MISC(stlabel), 0.0f, 0.0f);
	gtk_misc_set_alignment(GTK_MISC(dtlabel), 0.0f, 0.0f);
	gtk_misc_set_alignment(GTK_MISC(devlabel), 0.0f, 0.0f);
	gtk_misc_set_alignment(GTK_MISC(actlabel), 0.0f, 0.0f);
	
	gtk_progress_set_value (pbr, 0.0);
	gtk_label_set_text(stlabel,"Disconnected");
	gtk_label_set_text(devlabel,"");
	gtk_label_set_text(actlabel,"");
	gtk_label_set_text(dtlabel,"Not connected");
	
	gtk_main ();
	return 0;
}
