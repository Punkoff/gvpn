#! /bin/sh
#
# skeleton	example file to build /etc/init.d/ scripts.
#		This file should be used to construct scripts for /etc/init.d.
#
#		Written by Miquel van Smoorenburg <miquels@cistron.nl>.
#		Modified for Debian 
#		by Ian Murdock <imurdock@gnu.ai.mit.edu>.
#               Further changes by Javier Fernandez-Sanguino <jfs@debian.org>
#
# Version:	@(#)skeleton  1.9  26-Feb-2001  miquels@cistron.nl
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/bin/gvpn_daemon
NAME=gvpn_daemon
DESC="GVPN Daemon"

#test -x $DAEMON || exit 0

LOGDIR=/var/log/gvpn-daemon
PIDFILE=/var/run/$NAME.pid
DODTIME=1                   # Time to wait for the server to die, in seconds
RETVAL=0
# See how we were called.
case "$1" in
  start)
        echo -n "Starting $DESC: "

        r=`ps axw | grep "$NAME" | grep -v "grep" | wc |\
            awk '{print $1}'`
        if [ $r != "0" ]; then
          echo "$DESC is already running."
          RETVAL=1
        else
	  $DAEMON	
          RETVAL=0
        fi
	echo
        ;;
  stop)
        echo -n "Stopping $DESC: "

	gvpn_daemon -t
	pkill pptp
	pkill pppd
	pkill gvpn_daemon

        RETVAL=0
        ;;
  status)
        r=`ps axw | grep "$NAME" | grep -v "grep" | wc |\
            awk '{print $1}'`
        if [ $r != "0" ]; then
          echo "$DESC is running."
        else
          echo "$DESC is not running."
        fi
      
        RETVAL=0
        ;;
  restart)
        $0 stop
        $0 start
        RETVAL=$?
        ;;
  force-restart)
        $0 stop
        $0 start
        RETVAL=$?
        ;;
  reload)
        $0 restart
        RETVAL=$?
        ;;
  force-reload)
        $0 restart
        RETVAL=$?
        ;;  *)
        echo "Usage: gvpn-daemon {start|stop|status|restart}"
        exit 1
esac

exit $RETVAL
