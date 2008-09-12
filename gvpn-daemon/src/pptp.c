#include "pptp.h"

static char PPTPGateway[256];
static char PPTPLANGW[256];
static char PPTPLANDev[256];
static char PPTPDev[256];
static char PPTPLogin[256];
static char PPTPPassword[256];
static char PPTPLocalIP[256];
static char PPTPRemoteIP[256];
static char PPTPSent[256];
static char PPTPRcvd[256];

static int mode=0; //0=nothing 1=setpptpgw 2=setlogin 3=setpassword 4=setdev 5=setlangw
static int PPTPStatus=0; //0=disconnected;1=connecting;2=connected
extern int CommServerConnection;
extern int CommServerStatus;

void PPTPError(char* str) { //Send error message
	char buf[1024];
	sprintf(buf, "e%s", str);
	CommServerSendString(buf);
}
void PPTPNewClient() { //Notify newly connected client about situation
	PPTPError ("Internal error");
	if (PPTPStatus==0) CommServerSendString ("sd"); 
	else {
		CommServerSendString ("sc");
		char buffer[256];
		strcpy(buffer,"ol");
		strcat(buffer, PPTPLocalIP);
		CommServerSendString (buffer);
		strcpy(buffer,"or");
		strcat(buffer, PPTPRemoteIP);
		CommServerSendString (buffer);
		strcpy(buffer,"og");
		strcat(buffer, PPTPGateway);
		CommServerSendString (buffer);
		strcpy(buffer,"odDevice: /dev/");
		strcat(buffer, PPTPDev);
		CommServerSendString (buffer);
		CommServerSendString ("pc");
	}
	
	strcpy(PPTPLANGW,""); //Clear LAN gateway
}

FILE* PPTPFork(FILE* out, char* cmd) {
	int desc[2];
	
	pipe(desc); //Build a pipe
	out=fdopen(desc[0], "r");
	
	int pid=fork();
	
	if (pid==0) {
		dup2(desc[1],2); //Attach the pipe to stdout/err
		dup2(desc[1],1);
		system(cmd);
		exit(0);
	}
	
	sleep(3);
	
	return out;
}

void PPTPLaunch() { //Connect
	FILE *out=NULL;
	char buffer[1024];
	char cmd[1024];
	char tmp[1024];
	
	printf("\n");
	
	PPTPStatus=1;
	
	PPTPSaveDefRoute();
	if (strcmp(PPTPLANGW,"")!=0) PPTPGetLANGW(); //Get LAN GW if no custom specified
	
	sprintf(cmd,"pptp %s defaultroute replacedefaultroute nodetach name %s password %s", PPTPGateway, PPTPLogin, PPTPPassword);
	strcpy(tmp,"d");
	strcat(tmp,cmd);
	
	CommServerSendString("dCommand line:");
	CommServerSendString(tmp);
	CommServerSendString("mConnecting");
	
	printf("Created route to PPTP gateway\n");
	PPTPCreateModemRoute();
	
	printf("Launching PPTP\n");
	
	out=PPTPFork(out, cmd);
	if (out==NULL) {
		printf("Pipe creating failed!\n");
		CommServerSendString ("ei");
		PPTPTerminate ();
		return;
	}	
	
	PPTPStatus=2;
	
	while (!feof(out)) {
		strcpy(buffer,"");
		
		if (HasData(fileno(out),100)!= 0) {
			buffer[read(fileno(out), buffer, 1024)]=0;
			strcpy(tmp,"d");
			strcat(tmp,buffer);
			CommServerSendString(tmp);
			
			ParseFromPPTP(buffer);
		} 
		
		//or from client
		if (CommServerStatus!=2) {
			CommServerAccept ();
		}
		if (HasData(CommServerConnection,100) != 0) {
			char str[256];
			int n = CommServerReceiveString(str);
			if (n <= 0) {
				return;
			}
			
			PPTPProcess (str);
			if (strcmp(str,"abort")==0) {
				PPTPTerminate();
				return;
			}
		}
		
	}
}

void ParseFromPPTP(char* buffer) {
	char tmp[256];
	
	if (strstr(buffer,"Using interface")!=NULL) {   //Detected PPP device
		strcpy(buffer,"odDevice: /dev/");
		strcpy(PPTPDev,buffer+16);
		PPTPDev[4]=0;
		sprintf(tmp, "odDevice: /dev/%s", PPTPDev);
		CommServerSendString (tmp);
		CommServerSendString ("pa"); //Stage A
		CommServerSendString ("sConnecting");
		PPTPStatus=1;
		printf("Connecting: %s\n", PPTPDev);
	}
	if (strstr(buffer,"authentication succeeded")!=NULL) {
		CommServerSendString ("pb"); //Stage B
		CommServerSendString ("sAuthorized");
		printf("Authorized\n");
	}
	if (strstr(buffer,"authentication fail")!=NULL) {
		CommServerSendString ("p0"); //Stage 0
		CommServerSendString ("ea");
		printf("Authorization failure\n");
		PPTPTerminate ();
	}
	if (strstr(buffer,"local  IP address")!=NULL) { //IPs detected, connected
		char* p=0;
		
		CommServerSendString ("pc"); //Stage C
		CommServerSendString ("s");
		PPTPCreateModemRoute();
		
		//Get local IP
		strcpy(PPTPLocalIP, strstr(buffer,"local  IP")+strlen("local  IP address "));	
		if ((p=strstr(PPTPLocalIP," "))!=NULL) *p=0;
		if ((p=strstr(PPTPLocalIP,"\n"))!=NULL) *p=0;
		sprintf(tmp,"ol%s",PPTPLocalIP);
		CommServerSendString (tmp);
		
		//Get remote IP
		strcpy(PPTPRemoteIP, strstr(buffer,"remote IP")+strlen("remote IP address "));	
		if ((p=strstr(PPTPRemoteIP," "))!=NULL) *p=0;
		if ((p=strstr(PPTPRemoteIP,"\n"))!=NULL) *p=0;
		sprintf(tmp,"or%s",PPTPRemoteIP);
		CommServerSendString (tmp);
		
		//Send GW IP
		sprintf(tmp,"og%s",PPTPGateway);
		CommServerSendString (tmp);
		
		PPTPStatus=2;
		
		printf("Connection established\nLocal  IP: %s\nRemote IP: %s\n", PPTPLocalIP, PPTPRemoteIP);
	}
	if (strstr(buffer,"Terminating")!=NULL) { //PPTP down
		printf("Connection down\n");
		CommServerSendString ("p0"); //Stage 0
		PPTPTerminate();
		return;
	}
	if (strstr(buffer,"anon fatal")!=NULL) { //PPTP down
		printf("Fatal error:");
		if (strstr(buffer, "HOST NOT FOUND")!=NULL) {
			printf(" host not found");
			CommServerSendString ("ec");
		}
		CommServerSendString ("p0"); //Stage 0
		printf("\n");
		PPTPTerminate();
		return;
	}
	if (strstr(buffer,"terminat")!=NULL) { //PPTP down
		printf("Connection down\n");
		CommServerSendString ("ec");
		CommServerSendString ("p0"); //Stage 0
		PPTPTerminate();
		return;
	}
}

void PPTPCreateModemRoute() { //Create '<modem> via <gw> dev <dev>'
	char s[256];
	sprintf(s, "ip route add %s via %s dev %s > /dev/null", PPTPGateway, PPTPLANGW, PPTPLANDev);
	system(s);
}

void PPTPSaveDefRoute(){ //Save default route to /tmp/gvpn-def-route
	system("ip route|grep default > /tmp/gvpn-def-route");
	printf("Saved default route\n");
}

void PPTPGetLANGW() { //Get LAN gateway
	if (strcmp(PPTPLANGW,"")!=0) return; //Run away, if a custom gateway was specified
	
	system("cat /tmp/gvpn-def-route|cut -d' ' -f3 > /tmp/gvpn-lan-gw"); 
	FILE* fl=fopen("/tmp/gvpn-lan-gw","r");
	fgets(PPTPLANGW,256,fl);
	fclose(fl);
	
	printf("Lan gateway: %s",PPTPLANGW);
}

void PPTPDelDefRoute(){
	system("route del default > /dev/null");
	printf("Deleted default route\n");
}

void PPTPRestoreDefRoute(){
	char s[256];
	sprintf(s, "ip route add default via %s dev %s > /dev/null", PPTPLANGW, PPTPLANDev);
	system(s);
	printf("Restored default route\n");
}

void PPTPTerminate() {
	system("pkill pptp");
	system("pkill pppd");
	PPTPStatus=0;
	PPTPRestoreDefRoute();
}

void PPTPProcess(char* cmd){
	if (mode!=0){
		switch (mode) {
			case 1: strcpy(PPTPGateway,cmd);
			printf("Gateway: %s\n",PPTPGateway);
			break;
			case 2: strcpy(PPTPLogin,cmd);
			printf("Login: %s\n",PPTPLogin);
			break;
			case 3: strcpy(PPTPPassword,cmd);
			printf("Password: %s\n",PPTPPassword);
			break;
			case 4: strcpy(PPTPLANDev, cmd);
			printf("LAN Device: %s\n",PPTPLANDev);
			break;
			case 5: strcpy(PPTPLANGW, cmd);
			printf("LAN Gateway: %s\n",PPTPLANGW);
			break;
		}
		mode=0;
		return;
	}
	if (strcmp(cmd,"connect")==0){
		printf("\nConnecting\n");
		CommServerSendString("mLaunching PPTP");
		PPTPLaunch();
	}
	if (strcmp(cmd,"savedefroute")==0){
		CommServerSendString("dSaved default route");
		PPTPSaveDefRoute();
	}
	if (strcmp(cmd,"deldefroute")==0){
		CommServerSendString("dRemoved default route");		
		PPTPDelDefRoute();
	}
	if (strcmp(cmd,"setpptpgw")==0)		mode=1;
	if (strcmp(cmd,"setlogin")==0)		mode=2;
	if (strcmp(cmd,"setpassword")==0)   mode=3;
	if (strcmp(cmd,"setdev")==0)		mode=4;
	if (strcmp(cmd,"setlangw")==0)		mode=5;
	if (strcmp(cmd,"abort")==0)		
	{
		printf("\nDisonnecting\n");
		PPTPTerminate();
	}
	if (strcmp(cmd,"stat")==0){
		FILE *out;
		char buffer[1024];
		char c[1024];
		
		sprintf(buffer, "ifconfig %s|grep \"RX bytes\"|cut -d\' \' -f13,14", PPTPDev);
		out = popen(buffer, "r");
		while (!feof(out)) fgets(PPTPRcvd, 256, out);
		pclose(out);
		PPTPRcvd[strlen(PPTPRcvd)-2]=0; //Remove braces
		sprintf(c,"oc%s",PPTPRcvd+1);
		CommServerSendString (c);
		sprintf(buffer, "ifconfig %s|grep \"TX bytes\"|cut -d\' \' -f18,19", PPTPDev);
		out = popen(buffer, "r");
		while (!feof(out)) fgets(PPTPSent, 256, out);
		pclose(out);
		PPTPSent[strlen(PPTPSent)-2]=0; //Remove braces
		sprintf(c,"os%s",PPTPSent+1);
		CommServerSendString (c);
	}
}

