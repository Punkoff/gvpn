#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <sys/poll.h>

void error(char* str);
int HasData(int file, int timeout);
