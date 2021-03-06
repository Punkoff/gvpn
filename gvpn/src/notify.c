GtkStatusIcon* Notify;
char icon_c[256];
char icon_d[256];
char icon_n[256];
int icon_s=0;

void NotifyActivate() {
	if (GTK_WIDGET_VISIBLE(GTK_WIDGET(window))) {
		gtk_widget_hide(GTK_WIDGET(window));
	} else {
		gtk_widget_show(GTK_WIDGET(window));
	}
}

void NotifyPopup(GtkStatusIcon *status_icon, guint button,
				 guint32 activate_time, gpointer popUpMenu) {
					 
					 
	gtk_menu_popup(GTK_MENU(glade_xml_get_widget (gxml, "menuTray")), NULL,
				   NULL, gtk_status_icon_position_menu, Notify,
				   button, activate_time);
}

void NotifyInit() {
	strcpy(icon_n,RDir);
	strcat(icon_n,"/icon.png");
	strcpy(icon_d,RDir);
	strcat(icon_d,"/icon_d.png");
	strcpy(icon_c,RDir);
	strcat(icon_c,"/icon_c.png");
	Notify=gtk_status_icon_new_from_file (icon_n);
	g_signal_connect (G_OBJECT(Notify), "activate", G_CALLBACK (NotifyActivate),
		NULL);
	g_signal_connect (G_OBJECT(Notify), "popup-menu", G_CALLBACK (NotifyPopup),
		NULL);
}


void NotifyUpdate() {
	if (Connected==1 && icon_s!=1) {
		icon_s=1;
		gtk_status_icon_set_from_file (Notify, icon_c);
	}
	if (Connected==0 && icon_s!=2) {
		icon_s=2;
		gtk_status_icon_set_from_file (Notify, icon_d);
	}
	if (Connected==2 && icon_s!=0) {
		icon_s=0;
		gtk_status_icon_set_from_file (Notify, icon_n);
	}
	
	if (Connected==0) gtk_status_icon_set_tooltip (Notify, "GVPN - Disconnected");
	if (Connected==1) gtk_status_icon_set_tooltip (Notify, "GVPN - Connected");	
	if (Connected==2) gtk_status_icon_set_tooltip (Notify, "GVPN - Connecting...");	
}
