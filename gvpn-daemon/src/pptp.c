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

/*int begcmp(char* s1,char* s2) {
	int p=0;
	if (strlen(s1)<strlen(s2)) return -1;
	for (p=0;p<strlen(s2);p++) if (s1[p]!=s2[p]) return -1;
	return 0;
}*/

void PPTPNewClient() { //Notify newly connected client about situation
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

void PPTPLaunch() {
	FILE *out;
	char buffer[1024];
	char cmd[1024];
	char tmp[1024];
	
	PPTPStatus=1;
	
	PPTPSaveDefRoute();
	if (strcmp(PPTPLANGW,"")!=0) PPTPGetLANGW(); //Get LAN GW if no custom specified
	
	sprintf(cmd,"pptp %s defaultroute replacedefaultroute nodetach name %s password %s", PPTPGateway, PPTPLogin, PPTPPassword);
	strcpy(tmp,"d");
	strcat(tmp,cmd);
	printf("Command: %s\n",cmd);
	CommServerSendString("dCommand line:");
	CommServerSendString(tmp);
	CommServerSendString("mConnecting");
	
	PPTPCreateModemRoute();
	
	out = popen(cmd, "r");
	if (out==NULL) printf("No pipe!\n");
	
	PPTPStatus=2;
	
	while (!feof(out)) {
		strcpy(buffer,"");
		
		if (HasData(fileno(out),100)!= 0) {
			printf("got data\n");
			buffer[read(fileno(out), buffer, 1024)]=0;
			strcpy(tmp,"d");
			strcat(tmp,buffer);
			printf("pptp: %s",buffer);
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
			
			printf("Received: %s\n",str);
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
	}
	if (strstr(buffer,"authentication succeeded")!=NULL) {
		CommServerSendString ("pb"); //Stage B
		CommServerSendString ("sAuthorized");
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
	}
	if (strstr(buffer,"Terminating")!=NULL) { //PPTP down
		CommServerSendString ("p0"); //Stage 0
		PPTPTerminate();
		return;
	}
}

void PPTPCreateModemRoute() { //Create '<modem> via <gw> dev <dev>'
	char s[256];
	sprintf(s, "ip route add %s via %s dev %s", PPTPGateway, PPTPLANGW, PPTPLANDev);
	system(s);
}

void PPTPSaveDefRoute(){ //Save default route to /tmp/gvpn-def-route
	system("ip route|grep default > /tmp/gvpn-def-route");
	printf("Saved default route\n");
}

void PPTPGetLANGW() {
	if (strcmp(PPTPLANGW,"")!=0) return; //Run away, if a custom gateway was specified
	
	system("cat /tmp/gvpn-def-route|cut -d' ' -f3 > /tmp/gvpn-lan-gw"); 
	FILE* fl=fopen("/tmp/gvpn-lan-gw","r");
	fgets(PPTPLANGW,256,fl);
	fclose(fl);
	
	printf("Lan gateway: %s",PPTPLANGW);
}

void PPTPDelDefRoute(){
	system("route del default");
	printf("Deleted default route\n");
}

void PPTPRestoreDefRoute(){
	char s[256];
	sprintf(s, "ip route add default via %s dev %s", PPTPLANGW, PPTPLANDev);
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
			printf("Using password: %s\n",PPTPPassword);
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
	if (strcmp(cmd,"setpptpgw")==0)		mode=1;
	if (strcmp(cmd,"setlogin")==0)		mode=2;
	if (strcmp(cmd,"setpassword")==0)   mode=3;
	if (strcmp(cmd,"setdev")==0)		mode=4;
	if (strcmp(cmd,"setlangw")==0)		mode=5;
	if (strcmp(cmd,"abort")==0)		PPTPTerminate();
	
	if (strcmp(cmd,"stat")==0){
		FILE *out;
		char buffer[1024];
		char c[1024];
		strcpy(c,"ifconfig ");
		strcat(c,PPTPDev);
		out = popen(c, "r");
		while (!feof(out)) {
			fgets(buffer, 256, out);
			if (strstr(buffer,"RX bytes")!=NULL) { //Our line
				int bp=0,p=0;
				while (buffer[p]!='(') p++; 
				p+=1;
				while (buffer[p]!=')') {
					PPTPRcvd[bp]=buffer[p];
					p++;bp++;
				}
				PPTPRcvd[bp]=0;
				bp=0;							  //Sent
				while (buffer[p]!='(') p++; 
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

