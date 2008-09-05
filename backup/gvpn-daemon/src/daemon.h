#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include "common.h"


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_NAME "gvpn_daemon"
#define lockfile "/tmp/gvpn-daemon.lock"
#define RUN_AS_USER "daemon"


void daemonize();
