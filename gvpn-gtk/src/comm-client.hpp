/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */


#ifndef _COMM_CLIENT_HPP_
#define _COMM_CLIENT_HPP_
using namespace std;
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

class CommClient
{
public:
	int Init(string addr);
	void Close();
	int Connect();
	string Receive();
	int Send(string data);
	int ReceiveBytes(char* data, int maxlen);
protected:

private:
	int Socket;
	int Status; //0=uninitialized;1=idle;2=connected
	struct sockaddr_un Addr;
};

#endif // _COMM_CLIENT_HPP_
