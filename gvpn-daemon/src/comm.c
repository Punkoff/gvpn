#include "comm.h"
int CommServerSocket;
int CommServerConnection;
int CommServerStatus=0; //0=uninitialized;1=idle;2=connected
struct sockaddr_un CommServerAddr, CommServerFAddr;
int CommClientSocket;
int CommClientStatus=0; //0=uninitialized;1=idle;2=connected
struct sockaddr_un CommClientAddr;

   
int CommServerInit(char* addr) {
	int len;
	
    if ((CommServerSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("CommServer: Socket");
		CommServerStatus=0;
		return 0;
	}
    CommServerAddr.sun_family = AF_UNIX;
    strcpy(CommServerAddr.sun_path, addr);
    unlink(addr);
		
	len = sizeof(CommServerAddr.sun_family) + strlen(CommServerAddr.sun_path);
    if (bind(CommServerSocket, (struct sockaddr *)&CommServerAddr, len) < 0) {
        perror("CommServer: Bind");
    	CommServerStatus=0;
		return 0;
    }
	
	char sck[256];
	strcpy(sck,"chmod 666 ");
	strcat(sck,addr);
	system(sck);
	
	CommServerStatus=1;
	return 1;
}

int CommServerAccept() {
	int fromlen;
	
	
	if (listen(CommServerSocket, 5) < 0) {
        perror("CommServer: Listen");
   		CommServerStatus=0;
		return 0;
    }

	fromlen = sizeof(CommServerFAddr);
    if ((CommServerConnection = accept(CommServerSocket, (struct sockaddr *)&CommServerFAddr, &fromlen)) < 0) {
        perror("CommServer: Accept");
   		CommServerStatus=0;
		return 0;
    }
	
	CommServerStatus=2;
	
	return 1;
}

void CommServerClose() {
	close(CommServerSocket);
	close(CommServerConnection);
	CommServerStatus=1;
}

int CommServerReceive(char* data, int maxlen) {
	int res = recv(CommServerConnection, data, maxlen, 0);
	if (res==0) CommServerStatus=1; //Connection terminated
	if (res<0) CommServerStatus=0; //Connection error
	return res;
}

int CommServerReceiveString(char* data) {
	char c;
	c=0;
	int len=0;
	do {
		if (CommServerReceive(&c,1)<=0) {
			data[len]=0;
			return 0;
		}
		data[len]=c;
		len++;
	} while (c!=0);
	data[len+1]=0;

	return strlen(data);
}

int CommServerSend(char* data, int len) {
	return send(CommServerConnection, data, len, 0);
}

void CommServerSendString(char* data) {
	char str[256];
	strcpy(str,data);
	str[strlen(str)]=0;
	CommServerSend(str,strlen(str)+1);
}

int CommClientInit(char* addr) {
	CommClientStatus=0;
	
	if ((CommClientSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("CommClient: Socket");
        return 0;
    }

    CommClientAddr.sun_family = AF_UNIX;
    strcpy(CommClientAddr.sun_path, addr);

    CommClientStatus=1;
	return 1;
}

int CommClientConnect() {
	int len = 0;
	len = sizeof(CommClientAddr.sun_family) + strlen(CommClientAddr.sun_path);

	if (connect(CommClientSocket, (struct sockaddr *)&CommClientAddr, len) < 0) {
        perror("CommClient: Connect");
        return 0;
	}
	
	CommClientStatus=2;
	
	return 1;
}

void CommClientClose() {
	close(CommClientSocket);
	CommClientStatus=1;
}

int CommClientReceive(char* data, int maxlen) {
	int res = 0;
	res = recv(CommClientSocket, data, maxlen, 0);
	if (res==0) CommClientStatus=1; //Connection terminated
	if (res<0) CommClientStatus=0; //Connection error
	return res;
}

int CommClientReceiveString(char* data) {
	char c;
	c=0;
	int len=0;
	do {
		if (CommClientReceive (&c,1)<=0) {
			data[len]=0;
			return 0;
		}
		data[len]=c;
		len++;
	} while (c!=0);
	data[len+1]=0;

	return strlen(data);
}

int CommClientSend(char* data, int len) {
	return send(CommClientSocket, data, len, 0);
}
void CommClientSendString(char* data) {
	char str[256];
	strcpy(str,data);
	str[strlen(str)]=0;
	CommClientSend(str,strlen(str)+1);
}
