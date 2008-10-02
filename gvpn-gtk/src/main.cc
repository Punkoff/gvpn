/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) John Pankov 2008 <pankov@adsl.by>
 * 
 * main.cc is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.cc is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libglademm/xml.h>
#include <gtkmm.h>
#include <iostream>


#ifdef ENABLE_NLS
#  include <libintl.h>
#endif


#define GLADE_FILE "gvpn.glade"
   
using namespace Gtk;
using namespace std;
using namespace Glib;
using namespace Gnome;
using namespace Gnome::Glade;

#include "gvpn-ui.hpp"
#include "../config.h"

int main (int argc, char *argv[]) {
	cout << "GVPN-GTK " << VERSION << endl;
	cout << "Initializing UI..." << endl;
	GvpnUI ui(argc,argv);
	ui.Run();
	
	return 0;
}
