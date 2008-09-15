#include "cfg.h"

char CFILE[256],CVPNGW[256],CLogin[256],CPW[256],CSGW[256],CDV[256],CAC[256];
int CAR=0,CHC=0;

void CfgLoad() {
	char s[100];
	memset(s, 0, 100);
	//Load config
	strcpy(CFILE,getenv("HOME"));
	strcat(CFILE,"/.gvpn");
	FILE* cfg=fopen(CFILE,"r");
	if (cfg<=0) {
		cfg=fopen(CFILE,"w");
		if (cfg<=0) {
			ErrorBox ("Unable to create config file ~/.gvpn\nCheck file permissions",1);
		}
		fprintf(cfg,"\n\n\n\neth0\n");
		fclose(cfg);
		cfg=fopen(CFILE,"r");
		if (cfg<=0) {
			ErrorBox ("Unable to read config file ~/.gvpn\nCheck file permissions",1);
		}
	}
	fgets(CVPNGW,256,cfg);CVPNGW[strlen(CVPNGW)-1]=0;
	fgets(CLogin,256,cfg);CLogin[strlen(CLogin)-1]=0;
	fgets(CPW,256,cfg);CPW[strlen(CPW)-1]=0;
	fgets(CSGW,256,cfg);CSGW[strlen(CSGW)-1]=0;
	fgets(CDV,256,cfg);CDV[strlen(CDV)-1]=0;
	fgets(CAC,256,cfg);CAC[strlen(CAC)-1]=0;
	fgets(s,256,cfg);s[strlen(s)-1]=0;
	CAR=s[0]=='1'?1:0;
	fgets(s,256,cfg);s[strlen(s)-1]=0;
	CHC=s[0]=='1'?1:0;
	fclose(cfg);
}

void CfgSave() {
	strcpy(CFILE,getenv("HOME"));
	strcat(CFILE,"/.gvpn");
	FILE* cfg=fopen(CFILE,"w");
	if (cfg<=0) {
		ErrorBox ("Unable to create config file ~/.gvpn\nCheck file permissions",1);
	}
	fprintf(cfg,"%s\n%s\n%s\n%s\n%s\n%s\n%i\n%i\n\n",CVPNGW,CLogin,CPW,CSGW,CDV,CAC,CAR,CHC);
	fclose(cfg);
}
