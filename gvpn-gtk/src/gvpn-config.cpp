/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#include "gvpn-config.hpp"

bool GvpnConfig::Load(string name) {
	try {
		Name=name;
		string n=getenv("HOME");
		n += "/.gvpn.profiles/" + name + ".profile";
		ifstream f(n.c_str());
		f >> Gateway;
		f >> Login;
		f >> Password;
		f >> LanGateway;
		f >> Device;
		f.close();
	} catch (int e) {
		return false;
	}
}

bool GvpnConfig::Save() {
	// TODO: Add implementation here
}

int GvpnConfig::List(string res[]) {
	DIR *dp;
	int r=0;
    struct dirent *dirp;
	string dir = getenv("HOME");
	dir += "/.gvpn.profiles/";
    if((dp = opendir(dir.c_str())) == NULL) {
        return 0;
    } 

    while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name[0]!='.') res[r++]=dirp->d_name;
    }
    closedir(dp);
	return r;
}

