#include "daemon.h"

int fl_daemonize=1;
int fl_nodetach=0;

static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}


void daemonize()
{
    pid_t pid, sid, parent;
    int lfp = -1;

	//If daemon, exit
    if ( getppid() == 1 ) return;

	//Check lockfile
	lfp = open(lockfile,O_RDWR,0640);
	if ( lfp >= 0 ) error("lock file exists. Daemon is already running"); //TODO: restore lockfile check
	close(lfp);
	
	lfp = open(lockfile,O_RDWR|O_CREAT,0640);
        
    

    //Switch to 'daemon'
    if ( getuid() == 0 || geteuid() == 0 ) {
        struct passwd *pw = getpwnam(RUN_AS_USER);
        if ( pw ) {
            syslog( LOG_NOTICE, "setting user to " RUN_AS_USER );
            setuid( pw->pw_uid );
        }
    }

	//Signal handlers
    signal(SIGCHLD,child_handler);
    signal(SIGUSR1,child_handler);
    signal(SIGALRM,child_handler);

    //Fork
    pid = fork();
    if (pid < 0) error("unable to fork daemon");
    
	if (pid > 0) {
		//I'm parent
	    alarm(2);
        pause();
        exit(EXIT_FAILURE);
    }

    //I'm child
	parent = getppid();

    //Cancel signals
	signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

	//umask
    umask(0);

	//SID
    sid = setsid();
    if (sid < 0) error("unable to create a new session");

	//chdir
	if ((chdir("/")) < 0) error("unable to change directory");

	if (fl_nodetach==0) {
	    freopen( "/dev/null", "r", stdin);
	    freopen( "/dev/null", "w", stdout);
	    freopen( "/dev/null", "w", stderr);
	}

    kill( parent, SIGUSR1 );
	
	printf("Daemon running\n");
}
