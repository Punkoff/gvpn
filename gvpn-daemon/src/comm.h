#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int CommServerInit(char* addr);
int CommServerAccept();
void CommServerClose();
int CommServerReceive(char* data, int maxlen);
int CommServerReceiveString(char* data);
int CommServerSend(char* data, int len);
void CommServerSendString(char* data);
int CommClientInit(char* addr);
int CommClientConnect();
void CommClientClose();
int CommClientReceive(char* data, int maxlen);
int CommClientReceiveString(char* data);
int CommClientSend(char* data, int len);
void CommClientSendString(char* data);
