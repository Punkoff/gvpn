#include "common.h"

void error(char* str){
	printf("E: %s\n",str);
	exit(EXIT_FAILURE);
}
int HasData(int file, int timeout) {
	struct pollfd pfd;
	pfd.revents=0;
	pfd.fd=file;
	pfd.events=3;
	poll(&pfd,1,timeout);
	
	return ((pfd.revents & 3)!= 0)?1:0;
}
