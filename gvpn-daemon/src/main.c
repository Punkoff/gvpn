

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_NAME "gvpn_daemon"
#define lockfile "/tmp/gvpn-daemon.lock"
#define RUN_AS_USER "daemon"


#include <string.h>
#include "common.h"
#include "comm.h"
#include "daemon.h"
#include "pptp.h"
#include "config.h"

char buffer[256];
int fl_terminate=0;
extern int fl_nodetach;
extern int fl_daemonize;

void Process() {
	PPTPNewClient();
	
	int done = 0;
	do {
		char str[256];
	
		int n = CommServerReceiveString(str);
		if (strcmp(str,"terminate")==0) {
			PPTPTerminate ();
			fl_terminate=1;
			return;
		}
		if (n <= 0) {
			if (n < 0) perror("recv");
			done = 1;
		}
		
		if (!done) {
			PPTPProcess (str);  //if 'connect' will be received, we'll be stuck 
								//in here until 'abort'. See PPTPConnect()
		}
		
    } while (done==0);
}
 
int main( int argc, char *argv[] ) {
	//Parse command line
	int i;
	
	for (i=0;i<argc;i++) {
		if (strcmp(argv[i],"-f")==0) fl_daemonize=0;
		if (strcmp(argv[i],"--foreground")==0) fl_daemonize=0;
		if (strcmp(argv[i],"-n")==0) fl_nodetach=1;
		if (strcmp(argv[i],"--nodetach")==0) fl_nodetach=1;
		if (strcmp(argv[i],"-t")==0) fl_terminate=1;
		if (strcmp(argv[i],"--terminate")==0) fl_terminate=1;
		if (strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0) {
			printf("GVPN daemon\n");	
			printf("Usage: gvpn_daemon [-t|[-f|-n]]\n");
			printf("Optins:\n");
			printf("-f, --foreground	Do not daemonize\n");
			printf("-n, --nodetach		Do not detach from terminal\n");			
			printf("-t, --terminate		Terminate a running daemon\n");			
			printf("-h, --help			Show this help\n");			
		}
	}
	
	if (fl_terminate==1) {
		if (CommClientInit("/tmp/gvpn-daemon")==0) {
			perror("ServerInit");
			exit(1);
		} else {
			if (CommClientConnect()==0) {
				printf("Cannot connect to daemon\n");
				exit(1);
			} else {
				CommClientSendString ("terminate");
				printf("Terminating daemon\n");
			}
			CommClientClose();
		}
		exit(0);
	}
	
	remove("/tmp/gvpn-daemon");
	
	printf("GVPN Daemon %s\n",PACKAGE_VERSION);
	if (fl_daemonize==1) daemonize();
	
    while (fl_terminate==0) {
		printf("_______________________________\nCreating server...\n");
		if (CommServerInit("/tmp/gvpn-daemon")==0) {
			perror("ServerInit");
		} else {
			printf("Waiting for client...\n");
			if (CommServerAccept()==0) {
				perror("ServerAccept");
			} else {
				printf("Client connected\n");
				Process();
				printf("Client disconnected\n");
			}
			CommServerClose();
		}
	}
		
	printf("Terminating...\n\n");
		
    return 0;
}
