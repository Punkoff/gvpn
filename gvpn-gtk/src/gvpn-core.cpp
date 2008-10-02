/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gvpn-gtk
 * Copyright (C) John Pankov 2008 <pankov@adsl.by>
	 * 
 * gvpn-gtk is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 * 
 * gvpn-gtk is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gvpn-core.hpp"
using namespace std;
string GvpnCore::DaemonConnect() {
	if (Client.Init("/tmp/gvpn-daemon")==0) {
		return "Error: cannot open socket";
	}
	if (Client.Connect()==0) {
		return "Error: cannot connect to daemon";
	}
	
	string s = Client.Receive();
	Connected = (s == "sc")?1:0;
	
	cout << "Connected to daemon" << endl;
	return "Connected to daemon";
}

void GvpnCore::Connect() {
	Client.Send("setpptpgw");
	Client.Send(cfg.Gateway);
	Client.Send("setlogin");
	Client.Send(cfg.Login);
	Client.Send("setpassword");
	Client.Send(cfg.Password);
	Client.Send("setdev");
	Client.Send(cfg.Device);
	Client.Send("setlangw");
	Client.Send(cfg.LanGateway);
	Client.Send("connect");
	
}

void GvpnCore::Disconnect() {
	// TODO: Add implementation here
}

string GvpnCore::DeviceText() {
	// TODO: Add implementation here
}

string GvpnCore::StatusText() {
	if (Connected==1) return "<big><b>Connected</b></big>";
	if (Connected==0) return "<big><b>Disconnected</b></big>";
	if (Connected==2) return "<big><b>Connecting</b></big>";
}

string GvpnCore::ActionText() {
	return "";
}

string GvpnCore::InfoText() {
	// TODO: Add implementation here
}

string GvpnCore::LogText() {
	// TODO: Add implementation here
}


void GvpnCore::Configure(string profile) {
	cfg.Load(profile);
}
