
void on_tbAbout_clicked() {
	GtkAboutDialog* dlg=GTK_ABOUT_DIALOG(glade_xml_get_widget (gxml, "dlgAbout"));
	gtk_about_dialog_set_authors (dlg, authors);
	gtk_about_dialog_set_logo (dlg,gtk_window_get_icon(GTK_WINDOW(window)));
	gtk_widget_show (GTK_WIDGET(dlg));
	fatal=0;
	gtk_signal_connect (GTK_OBJECT (dlg), "response", GTK_SIGNAL_FUNC (ErrorBox_Signal),
						NULL);
}


void on_tbConnect_clicked() { //Connect 
	ConsoleAdd("----------------------\n");
	ConsoleAdd("Starting connection...\n");
	
	CommClientSendString("setpptpgw");
	CommClientSendString(CVPNGW);
	CommClientSendString("setlogin");
	CommClientSendString(CLogin);
	CommClientSendString("setpassword");
	CommClientSendString(CPW);
	CommClientSendString("setdev");
	CommClientSendString(CDV);
	CommClientSendString("setlangw");
	CommClientSendString(CSGW);
	CommClientSendString("connect");
	
	Connected=2;
	ManualDisc = 0;
}

void on_tbDisconnect_clicked() { //Disconnect
	CommClientSendString ("abort");
	gtk_progress_set_value (pbr, 0.0);
	gtk_label_set_text(stlabel,"Disconnected");
	gtk_label_set_text(devlabel,"");
	gtk_label_set_text(actlabel,"");
	gtk_label_set_text(dtlabel,"Not connected");
	Connected=0;
	ConsoleAdd("Disconnected\n");
	ManualDisc=1;
}

void on_tbSettings_clicked() {
	printf("Settings\n");
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry1")),CVPNGW);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry2")),CLogin);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry3")),CPW);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "checkbutton1")), (strcmp(CSGW,"")==0)?FALSE:TRUE);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry5")),CSGW);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkRA")), (strcmp(CAC,"")==0)?FALSE:TRUE);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry4")),CAC);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkReconnect")), (CAR==0)?FALSE:TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "checkbutton2")), (CHC==0)?FALSE:TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkDebug")), (CDBG==0)?FALSE:TRUE);
	gtk_widget_show (wndSettings);
}

void on_cmdSetOK_clicked() {
	strcpy(CVPNGW,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry1"))));
	strcpy(CLogin,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry2"))));
	strcpy(CPW,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry3"))));
	strcpy(CDV,gtk_combo_box_get_active_text(cbx));
	strcpy(CSGW,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry5"))));
	strcpy(CAC,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget (gxml, "entry4"))));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "checkbutton1")))==FALSE) strcpy(CSGW,"");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkRA")))==FALSE) strcpy(CAC,"");
	CAR=(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkReconnect"))))?1:0;
	CHC=(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "checkbutton2"))))?1:0;
	CDBG=(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget (gxml, "chkDebug"))))?1:0;
	gtk_widget_hide (wndSettings);
	if (strlen(CVPNGW)==0)
	{
		ErrorBox ("Please enter PPTP gateway address!",0);
		CfgLoad();
		return;
	}
	if (strlen(CLogin)==0)
	{
		ErrorBox ("Please enter login!",0);
		CfgLoad();
		return;
	}
	if (strlen(CPW)==0)
	{
		ErrorBox ("Please enter password!",0);
		CfgLoad();
		return;
	}

	CfgSave();
}

void on_cmdSetCancel_clicked() {
	gtk_widget_hide (wndSettings);
}
