/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */


#ifndef _GVPN_CONFIG_HPP_
#define _GVPN_CONFIG_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <sys/stat.h> 
#include <stdlib.h>

using namespace std;

class GvpnConfig
{
public:
	bool Load(string name);
	bool Save();
	static int List(string res[]);
	string Gateway;
	string Login;
	string Password;
	string Device;
	string LanGateway;
	bool Hide;
	bool Reconnect;
	bool Debug;
	string RunAfter;
	string Name;
protected:

private:

};

#endif // _GVPN_CONFIG_HPP_
