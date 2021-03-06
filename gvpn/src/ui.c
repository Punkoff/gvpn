
GtkLabel *stlabel;
GtkLabel *actlabel;
GtkLabel *dtlabel;
GtkLabel *devlabel;
GtkProgress *pbr;
GtkComboBox *cbx;
GtkToolButton *tbConnect;
GtkToolButton *tbDisconnect;

char Errors[10][1024];


void UpdateDataLabel(){
	char s[256];
	if (Connected==1){
		strcpy(s,_("Gateway IP: "));
		strcat(s,IPG);
		strcat(s,_("\nLocal IP: "));
		strcat(s,IPL);
		strcat(s,_("\nRemote IP: "));
		strcat(s,IPR);
		strcat(s,_("\nData received: "));
		strcat(s,SRcvd);
		strcat(s,_("\nData sent: "));
		strcat(s,SSent);
		gtk_label_set_text(dtlabel,s);
		gtk_label_set_text(stlabel,_("Connected"));
		gtk_label_set_text(actlabel,"Idle");
	}
	if (Connected==0) {
		gtk_label_set_text(stlabel,_("Disconnected"));
		gtk_label_set_text(dtlabel,_("No info available"));
		gtk_label_set_text(actlabel,_("Idle"));
	}
	if (Connected==2) gtk_label_set_text(stlabel,_("Connecting..."));
	
}

void UIInit() {
	//Set styles
	stlabel=GTK_LABEL(glade_xml_get_widget (gxml, "stlabel"));
	dtlabel=GTK_LABEL(glade_xml_get_widget (gxml, "dtlabel"));
	devlabel=GTK_LABEL(glade_xml_get_widget (gxml, "devlabel"));
	pbr=GTK_PROGRESS(glade_xml_get_widget (gxml, "pbr"));
	actlabel=GTK_LABEL(glade_xml_get_widget (gxml, "actlabel"));
	tbConnect=GTK_TOOL_BUTTON(glade_xml_get_widget (gxml, "tbConnect"));
	tbDisconnect=GTK_TOOL_BUTTON(glade_xml_get_widget (gxml, "tbDisconnect"));
	
	PangoFontDescription *font_desc = pango_font_description_from_string ("sans serif 16");
	gtk_widget_modify_font (GTK_WIDGET(stlabel), font_desc);
	pango_font_description_free (font_desc);
	font_desc = pango_font_description_from_string ("sans serif bold 8");
	gtk_widget_modify_font (GTK_WIDGET(devlabel), font_desc);
	pango_font_description_free (font_desc);
	
	//gtk_window_set_icon_from_file(GTK_WINDOW(window), "icon.png", NULL);
	cbx=GTK_COMBO_BOX(gtk_combo_box_new_text ());
	gtk_table_attach(GTK_TABLE(glade_xml_get_widget (gxml, "table1")), GTK_WIDGET(cbx), 1,2,3,4, GTK_FILL,GTK_EXPAND,0,0);
	gtk_widget_show (GTK_WIDGET(cbx));
	
	//Read devices list
	FILE* n=fopen("/etc/network/interfaces","r");
	int i=0,a=0;//Saved device
	while (!feof(n)) {
		char buf[256];
		fgets(buf,256,n);
		if (buf[0]=='i' && buf[1]=='f' && buf[2]=='a')  {
			int p=6;
			while (buf[p]!=' ' && buf[p]!='\n') p++;
			buf[p]=0;
			if (strcmp(CDV,buf+6)==0) a=i;
			gtk_combo_box_append_text(cbx, buf+6);
			i++;
		}
	}
	fclose(n);
	gtk_combo_box_set_active (cbx, a);
	
	gtk_progress_set_value (pbr, 0.0);
	gtk_label_set_text(stlabel,"Disconnected");
	gtk_label_set_text(devlabel,"");
	gtk_label_set_text(actlabel,"");
	gtk_label_set_text(dtlabel,"Not connected");
	
	GtkNotebook* nbk=GTK_NOTEBOOK(glade_xml_get_widget (gxml, "notebook"));
	gtk_notebook_remove_page(nbk, 2);
	
	strcpy(Errors[0],"Internal error");
	strcpy(Errors[1],"Connection error.\nServer unavailable.");
	strcpy(Errors[2],"Authentication error.\nCannot login");
	strcpy(Errors[3],"Internal error");
	
}

void SetButtons() {
	if (Connected>0) {
		gtk_widget_set_sensitive (GTK_WIDGET(tbConnect),FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET(tbDisconnect),TRUE);
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET(tbConnect),TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET(tbDisconnect),FALSE);
	}
}
