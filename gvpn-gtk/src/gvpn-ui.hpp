/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef _GVPN_UI_HPP_
#define _GVPN_UI_HPP_

#include <libglademm/xml.h>
#include <gtkmm.h>
#include <iostream>
#include "gvpn-core.hpp"
#include "gvpn-config.hpp"

class GvpnUI
{
	public:
		GvpnUI(int argc, char* argv[]);
		void Init();
		void Run();
		void ShowSettings();
		void Connect();
		void Update();
		void Setup();
		void ConsoleAdd(string str, string type);
		GvpnCore core;
		Gtk::Main kit;
		Glib::RefPtr<Gnome::Glade::Xml> Glade;
		Gtk::Window* wndMain;
		Gtk::Window* wndSettings;
		Gtk::AboutDialog* wndAbout;
		Gtk::ToolButton* tbConnect;
		Gtk::ToolButton* tbDisconnect;
		Gtk::ToolButton* tbSettings;
		Gtk::ToolButton* tbAbout;
		Gtk::Label* lblStatus;
		Gtk::Label* lblAction;
		Gtk::Label* lblDevice;
		Gtk::Label* lblInfo;
		Gtk::TextView* Console;
		Gtk::MenuItem* miConnect;
		Gtk::MenuItem* miDisconnect;
		Gtk::MenuItem* miQuit;
		Gtk::MenuItem* miSettings;
		Gtk::ComboBox* cbxProfile;
		Glib::RefPtr<Gtk::ListStore> Profiles;
	protected:
		void on_Settings_Pressed();
		void on_About_Pressed();
		void on_About_Response();
		void on_wndMain_Mapped();
		void on_kit_run();
		int on_Stat_TO();
		int on_Update_TO();
		void on_Quit_pressed();
		void on_Connect_pressed();
		void on_Disconnect_pressed();
		void on_Profile_changed();
		class ModelColumns : public Gtk::TreeModel::ColumnRecord
		{
			public:
				ModelColumns()	{ add(name); }
				Gtk::TreeModelColumn<Glib::ustring> name;
		};
		ModelColumns Columns;

	private:
		void Extract();
		Glib::RefPtr<Gtk::TextBuffer::Tag> tag_Error,tag_Success,tag_Info;
};

#endif // _GVPN_UI_HPP_

