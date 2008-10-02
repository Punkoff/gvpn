#include "gvpn-ui.hpp"

using namespace Gnome::Glade;
using namespace std;
using namespace Gtk;


const char *authors[] =
		{
			"John Pankov <pankov@adsl.by>",
			NULL
		};
	

GvpnUI::GvpnUI (int argc, char* argv[]):kit(argc, argv) { };

void GvpnUI::Init() {
	try
	{
		cout << "Loading UI XML..." << endl;
		Glade = Gnome::Glade::Xml::create("gvpn.glade");
		chdir("/usr/share/gvpn");
		Glade->get_widget("wndMain", wndMain);
		wndMain->set_default_icon_from_file("icon.png");
	}
	catch(const XmlError& ex)
    {
		cerr << ex.what() << std::endl;
		return;
	}
}

void GvpnUI::Extract() {
	Glade->get_widget("wndSettings", wndSettings);
	Glade->get_widget("wndAbout", wndAbout);
	Glade->get_widget("tbConnect", tbConnect);
	Glade->get_widget("tbDisconnect", tbDisconnect);
	Glade->get_widget("tbSettings", tbSettings);
	Glade->get_widget("tbAbout", tbAbout);
	Glade->get_widget("lblStatus", lblStatus);
	Glade->get_widget("lblAction", lblAction);
	Glade->get_widget("Console", Console);
	Glade->get_widget("miConnect", miConnect);
	Glade->get_widget("miDisconnect", miDisconnect);
	Glade->get_widget("miExit", miQuit);
	Glade->get_widget("miSettings", miSettings);
	Glade->get_widget("cbxProfile", cbxProfile);

}

void GvpnUI::Connect() {
	tbSettings->signal_clicked().connect(sigc::mem_fun(*this, &GvpnUI::on_Settings_Pressed));
	tbAbout->signal_clicked().connect(sigc::mem_fun(*this, &GvpnUI::on_About_Pressed));
	wndMain->signal_map().connect(sigc::mem_fun(*this, &GvpnUI::on_wndMain_Mapped));
	kit.signal_run().connect(sigc::mem_fun(*this, &GvpnUI::on_kit_run));
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GvpnUI::on_Update_TO), 100);
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GvpnUI::on_Stat_TO), 2000);
	miQuit->signal_activate().connect(sigc::mem_fun(*this, &GvpnUI::on_Quit_pressed));
	tbConnect->signal_clicked().connect(sigc::mem_fun(*this, &GvpnUI::on_Connect_pressed));
	tbDisconnect->signal_clicked().connect(sigc::mem_fun(*this, &GvpnUI::on_Disconnect_pressed));
	cbxProfile->signal_changed().connect(sigc::mem_fun(*this, &GvpnUI::on_Profile_changed));
	miConnect->signal_activate().connect(sigc::mem_fun(*this, &GvpnUI::on_Connect_pressed));
	miDisconnect->signal_activate().connect(sigc::mem_fun(*this, &GvpnUI::on_Disconnect_pressed));
}

void GvpnUI::Setup() {
	//Create tags for console formatting
	Glib::RefPtr<Gtk::TextBuffer> buf = Console->get_buffer();
	Glib::RefPtr<TextBuffer::TagTable> 	tt= buf->get_tag_table();
	tag_Error = Gtk::TextBuffer::Tag::create();
	tag_Error->property_foreground() = "red";
	tag_Success = Gtk::TextBuffer::Tag::create();
	tag_Success->property_foreground() = "darkgreen";
	tag_Info = Gtk::TextBuffer::Tag::create();
	tag_Info->property_foreground() = "gray";
	tt->add(tag_Error);
	tt->add(tag_Success);
	tt->add(tag_Info);
	
	//Fill preset menu
	string ps[512];
	int c = GvpnConfig::List(ps);
	Profiles = Gtk::ListStore::create(Columns);
	Gtk::TreeModel::Row row = *(Profiles->append());
	for (int i=0;i<c;i++) {
		row[Columns.name] = ps[i].substr(0, ps[i].find(".profile"));
		row = *(Profiles->append());
	}
	cbxProfile->set_model(Profiles);
	cbxProfile->pack_start(Columns.name);
	cbxProfile->set_active(0);
	//on_Profile_changed ();
}

void GvpnUI::Run() {
	Init();
	
	if (wndMain)
	{
		Extract();
		Connect();
		Setup();
		cout << "UI created" << endl;
		kit.run();
	}
}

void GvpnUI::Update() {
	if (core.Connected==1) 
	{
		tbConnect->set_sensitive(false);
		tbDisconnect->set_sensitive(true);
	}
	if (core.Connected==0) 
	{
		tbConnect->set_sensitive(true);
		tbDisconnect->set_sensitive(false);
	}
	lblStatus->set_markup(core.StatusText());		
	lblAction->set_markup(core.ActionText());
	miConnect->set_sensitive(tbConnect->is_sensitive());
	miDisconnect->set_sensitive(tbDisconnect->is_sensitive());
}
void GvpnUI::ConsoleAdd(string str,string type) {
	Glib::RefPtr<Gtk::TextBuffer> buf = Console->get_buffer();
	Gtk::TextIter it = buf->get_iter_at_line(buf->get_line_count()+1);
	if (type=="e") buf->insert_with_tag(it, str + "\n" , tag_Error);
	if (type=="s") buf->insert_with_tag(it, str + "\n" , tag_Success);
	if (type=="i") buf->insert_with_tag(it, str + "\n" , tag_Info);
	it = buf->get_iter_at_line(buf->get_line_count()+1);
	Console->scroll_to_iter(it,0);
}
void GvpnUI::on_Settings_Pressed () {
	wndSettings->show();
}

void GvpnUI::on_About_Pressed () {
	wndAbout->set_authors(authors);
	wndAbout->set_icon(wndMain->get_icon());
	wndAbout->set_logo(wndMain->get_icon());
	wndAbout->run();
	wndAbout->hide();
}

void GvpnUI::on_About_Response() {
	wndAbout->hide();
}

void GvpnUI::on_wndMain_Mapped() {
	ConsoleAdd(core.DaemonConnect(),"s");
}
void GvpnUI::on_kit_run() {
	wndMain->show();
}
int GvpnUI::on_Stat_TO() {
	ConsoleAdd("Stat.","i");
	return 1;
}
int GvpnUI::on_Update_TO() {
	Update();
	return 1;
}
void GvpnUI::on_Quit_pressed() {
	cout << "Shutting down client" << endl;
	kit.quit();
	exit(0);
}
void GvpnUI::on_Connect_pressed() {
	core.Connect();
}
void GvpnUI::on_Disconnect_pressed() {
	core.Disconnect();
}
void GvpnUI::on_Profile_changed() {
	Gtk::TreeModel::iterator iter = cbxProfile->get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if(row)
		{
			Glib::ustring name = row[Columns.name];
			core.Configure(name);
			cout << "Selected profile: " << name << endl;
		}
	}
}




