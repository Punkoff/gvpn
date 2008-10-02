#include "comm-client.hpp"

int CommClient::Init(string addr) {
	Status=0;
	
	if ((Socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("CommClient::Init");
        return 0;
    }

    Addr.sun_family = AF_UNIX;
    strcpy(Addr.sun_path, addr.c_str());

    Status=1;
	return 1;
}

void CommClient::Close() {
	close(Socket);
	Status=1;
}

int CommClient::Connect() {
	int len = 0;
	len = sizeof(Addr.sun_family) + strlen(Addr.sun_path);

	if (connect(Socket, (struct sockaddr *)&Addr, len) < 0) {
		perror("CommClient::Connect");
        return 0;
	}
	
	Status=2;
	
	return 1;
}


int CommClient::ReceiveBytes(char* data, int maxlen) {
	int res = 0;
	res = recv(Socket, data, maxlen, 0);
	if (res==0) Status=1; //Connection terminated
	if (res<0) Status=0; //Connection error
	return res;
}

string CommClient::Receive() {
	char c;
	c=0;
	int len=0;
	char data[1024];
	
	do {
		if (ReceiveBytes (&c,1)<=0) {
			data[len]=0;
			return 0;
		}
		data[len]=c;
		len++;
	} while (c!=0);
	data[len+1]=0;

	return string(data);
}

int CommClient::Send(string data) {
	char str[256];
	strcpy(str,data.c_str());
	str[strlen(str)]=0;
	return send(Socket, str, strlen(str)+1, 0);
}


