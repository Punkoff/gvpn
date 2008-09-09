#include <sys/poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"
#include "common.h"

void PPTPNewClient();
void PPTPLaunch();
void PPTPGetDefRoute(char* s);
void PPTPSaveDefRoute();
void PPTPDelDefRoute();
void PPTPProcess(char* cmd);
void PPTPGetLANGW();
void PPTPCreateModemRoute();
void PPTPTerminate();
void ParseFromPPTP();
