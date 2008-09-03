#include "cfg.h"

char CFILE[256],CVPNGW[256],CLogin[256],CPW[256],CSGW[256],CDV[256];

void CfgLoad() {
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
	fclose(cfg);
	
}

void CfgSave() {
	strcpy(CFILE,getenv("HOME"));
	strcat(CFILE,"/.gvpn");
	FILE* cfg=fopen(CFILE,"w");
	if (cfg<=0) {
		ErrorBox ("Unable to create config file ~/.gvpn\nCheck file permissions",1);
	}
	fprintf(cfg,"%s\n%s\n%s\n%s\n%s\n",CVPNGW,CLogin,CPW,CSGW,CDV);
	fclose(cfg);
}