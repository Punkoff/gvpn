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

int begcmp(char* s1,char* s2) {
	int p=0;
	if (strlen(s1)<strlen(s2)) return -1;
	for (p=0;p<strlen(s2);p++) if (s1[p]!=s2[p]) return -1;
	return 0;
}

void PPTPNewClient() {
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

	}
	
	strcpy(PPTPLANGW,""); //Clear LAN gateway
}

void PPTPLaunch() {
	FILE *out;
	char buffer[1024];
	char cmd[1024];
	char tmp[1024];
	
	PPTPStatus=1;
	
	PPTPSaveDefRoute();
	if (strcmp(PPTPLANGW,"")!=0) PPTPGetLANGW(); //Get LAN GW if no custom specified
	
	strcpy(cmd,"pptp ");
	strcat(cmd,PPTPGateway);
	strcat(cmd," defaultroute replacedefaultroute nodetach name ");
	strcat(cmd,PPTPLogin);
	strcat(cmd," password ");
	strcat(cmd,PPTPPassword);
	strcpy(tmp,"d");
	strcat(tmp,cmd);
	CommServerSendString("dCommand line:");
	CommServerSendString(tmp);
	CommServerSendString("mConnecting");
		
	PPTPCreateModemRoute();
		
	//FCSend("gvpn","Launching PPTP");
	out = popen(cmd, "r");
	
	PPTPStatus=2;
	while (!feof(out)) {
		strcpy(buffer,"");
		
		//---------------------------------------------------
		//check if there is something from pptp
		struct pollfd pfd;
		pfd.revents=0;
		pfd.fd=fileno(out);
		pfd.events=3;
		poll(&pfd,1,100);
		
		if ((pfd.revents & 3)!= 0) {
			buffer[read(fileno(out), buffer, 1024)]=0;
			strcpy(tmp,"d");
			strcat(tmp,buffer);
			printf("pptp: %s",buffer);
			CommServerSendString(tmp);
			if (begcmp(tmp+1,"Using interface")==0) {   //Detect PPP device
				strcpy(buffer,"odDevice: /dev/");
				strcpy(PPTPDev,tmp+17);
				PPTPDev[4]=0;
				strcat(buffer, PPTPDev);
				CommServerSendString (buffer);
				CommServerSendString ("pa");
				CommServerSendString ("sConnecting");
				PPTPStatus=1;
			}
			if (begcmp(tmp+1,"CHAP authentication succeeded")==0) {
				CommServerSendString ("pb");
				CommServerSendString ("sAuthorized");
			}
			if (begcmp(tmp+1,"PAP authentication succeeded")==0) {
				CommServerSendString ("pb");
				CommServerSendString ("sAuthorized");
			}
			if (begcmp(tmp+1,"replacing")==0) { //IPs
				CommServerSendString ("pc");
				CommServerSendString ("s");
				PPTPCreateModemRoute();
				//Get local IP
				strcpy(buffer,"ol");
				int bp=0,p=0;
				while (begcmp(tmp+p,"local  IP")!=0) p++;
				p+=18;
				while (tmp[p]!='\n') {
					PPTPLocalIP[bp]=tmp[p];
					p++;bp++;
				}
				PPTPLocalIP[bp]=0;
				strcat(buffer,PPTPLocalIP);
				CommServerSendString (buffer);
				//Get remote IP
				strcpy(buffer,"or");
				bp=0;
				while (begcmp(tmp+p,"remote IP")!=0) p++;
				p+=18;
				while (tmp[p]!='\n') {
					PPTPRemoteIP[bp]=tmp[p];
					p++;bp++;
				}
				PPTPRemoteIP[bp]=0;
				strcat(buffer,PPTPRemoteIP);
				CommServerSendString (buffer);
				strcpy(buffer,"og");
				strcat(buffer, PPTPGateway);
				CommServerSendString (buffer);
		
				PPTPStatus=2;
			}
			if (begcmp(tmp+1,"Terminating")==0) { //PPTP down
				CommServerSendString ("p0");
				PPTPTerminate();
				return;
			}
		} 

		//or from client
		pfd.fd=CommServerConnection;
		pfd.events=3;
		pfd.revents=0;
		poll(&pfd,1,100);
		if ((pfd.revents & 3) != 0) {
			char str[256];
			int n = CommServerReceiveString(str);
			if (n <= 0) {
				return;
			}
		
			printf("Received: %s\n",str);
			PPTPProcess (str);
			if (strcmp(str,"abort")==0) {
				PPTPTerminate();
				return;
			}
		}

	}
}

void PPTPGetDefRoute(char* s){ //Get default LAN route
	FILE *out;
	char b[256];
	char res[256];
	
	out = popen("ip route" , "r");
	
	while (!feof(out)) {
		fgets(b,256, out);
		if (b[0]=='d' && b[1]=='e' && b[2]=='f') {
			memcpy(res,b,256);
		}
	}

	memcpy(s, res, 256);
	return;
}

void PPTPCreateModemRoute() { //Create '<modem> via <gw> dev <dev>'
	char s[256];
	strcpy(s, "ip route add ");
	strcat(s, PPTPGateway);
	strcat(s, " via ");
	strcat(s, PPTPLANGW);
	strcat(s, " dev ");
	strcat(s, PPTPLANDev);
	system(s);
}

void PPTPSaveDefRoute(){ //Save default route to /tmp/gvpn-def-route
	char route[256];
	PPTPGetDefRoute(route);
	system("echo > /tmp/gvpn-def-route");
	FILE* fl=fopen("/tmp/gvpn-def-route","w");
	fprintf(fl, "%s",route);
	fclose(fl);
	printf("Saved default route\n");
}

void PPTPGetLANGW() {
	FILE* fl=fopen("/tmp/gvpn-def-route","r");
	char s[256];
	fgets(s,256,fl);
	fclose(fl);
	int p=0;
	while (p<strlen(s) && begcmp(s+p,"via")!=0) p++;
	p+=4;
	int bp=0;
	strcpy(PPTPLANGW,"");
	while (s[p]!=' ' && p<strlen(s)) {
		PPTPLANGW[bp]=s[p];
		p++;bp++;
	}
	PPTPLANGW[bp]=0;
	printf("Lan gateway: %s",PPTPLANGW);
}

void PPTPDelDefRoute(){
	system("route del default");
	printf("Deleted default route\n");
}

void PPTPRestoreDefRoute(){
	char s[256];
	strcpy(s, "ip route add default via ");
	strcat(s, PPTPLANGW);
	strcat(s, " dev ");
	strcat(s, PPTPLANDev);
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
					printf("Using gateway: %s\n",PPTPGateway);
					break;
			case 2: strcpy(PPTPLogin,cmd);
					printf("Using login: %s\n",PPTPLogin);
					break;
			case 3: strcpy(PPTPPassword,cmd);
					printf("Using gateway: %s\n",PPTPPassword);
					break;
			case 4: strcpy(PPTPLANDev, cmd);
					printf("Using device: %s\n",PPTPLANDev);
					break;
			case 5: strcpy(PPTPLANGW, cmd);
					printf("Using LAN gateway: %s\n",PPTPLANGW);
					break;
		}
		mode=0;
		return;
	}
	if (strcmp(cmd,"connect")==0){
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
	if (strcmp(cmd,"setpptpgw")==0){
		mode=1;
	}
	if (strcmp(cmd,"setlogin")==0){
		mode=2;
	}
	if (strcmp(cmd,"setpassword")==0){
		mode=3;
	}
	if (strcmp(cmd,"setdev")==0){
		mode=4;
	}
	if (strcmp(cmd,"setlangw")==0){
		mode=5;
	}
	if (strcmp(cmd,"abort")==0){
		PPTPTerminate();
	}
	if (strcmp(cmd,"stat")==0){
		FILE *out;
		char buffer[1024];
		char c[1024];
		strcpy(c,"ifconfig ");
		strcat(c,PPTPDev);
		out = popen(c, "r");
		while (!feof(out)) {
			fgets(buffer, 256, out);
			if (begcmp(buffer+10,"RX bytes")==0) { //Our line
				int bp=0,p=0;
				while (begcmp(buffer+p,"(")!=0) p++; //Received
				p+=1;
				while (buffer[p]!=')') {
					PPTPRcvd[bp]=buffer[p];
					p++;bp++;
				}
				PPTPRcvd[bp]=0;
				bp=0;							  //Sent
				while (begcmp(buffer+p,"(")!=0) p++; 
				p+=1;
				while (buffer[p]!=')') {
					PPTPSent[bp]=buffer[p];
					p++;bp++;
				}
				PPTPSent[bp]=0;
			}
		}
		printf("Received: %s, sent: %s\n",PPTPRcvd,PPTPSent);//Send data to the client
		strcpy(buffer,"os");
		strcat(buffer,PPTPSent);
		CommServerSendString (buffer);
		strcpy(buffer,"oc");
		strcat(buffer,PPTPRcvd);
		CommServerSendString (buffer);
	}
}

