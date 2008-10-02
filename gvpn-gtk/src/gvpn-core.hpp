#ifndef _CORE_HPP_
#define _CORE_HPP_
#include "comm-client.hpp"
#include "gvpn-config.hpp"
#include <string>
#include <iostream>

class GvpnCore
{
public:
	CommClient Client;
	string DaemonConnect();
	void Connect();
	void Disconnect();
	void Configure(string profile);
	std::string DeviceText();
	std::string StatusText();
	std::string ActionText();
	std::string InfoText();
	std::string LogText();
	int Connected;
protected:

private:
	string device;
	string b_rec, b_sent, ip_l, ip_r, ip_g;
	GvpnConfig cfg;
};

#endif // _CORE_HPP_
