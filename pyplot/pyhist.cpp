#include "../utils/utils.hh"
#include "pyhist.hh"
#include <cstring>
#include <cstdlib>

//pyhist.hh.
PyHist::PyHist(const char* namei, enum pipe_type pipe){
	pipe_state = pipe;
	verify_dir("FIGS/");
	chdir("FIGS/");
	pypipe = popen("python", "w");
	assrt(pypipe != NULL);
	chdir("..");
	
	assrt(strlen(namei)<25);
	strcpy(name, namei);

	state = 0;

	for(int i=0; i < MAX_NUM_PYPLT_CMDS; i++)
		cmd[i] = new char[MAX_CMD_PYPLT_LEN];
	
	cmdnum = 0;

	savedata = 0;	

	sprintf(cmdstr, "import matplotlib as mpl\n");
	issue_command(cmdstr);
	sprintf(cmdstr, "mpl.rcParams['font.size'] = 18\n");
	issue_command(cmdstr);

	/*
	 * Changes backend if PLTOFF.
	 */
	if(pipe_state == PLTOFF){
		sprintf(cmdstr, "mpl.use('PDF', warn = True)\n");
		issue_command(cmdstr);
	}

	sprintf(cmdstr, "from matplotlib import pyplot as plt\n");
	issue_command(cmdstr);

	sprintf(cmdstr, "from numpy import genfromtxt\n");
	issue_command(cmdstr);

	sprintf(cmdstr, "fig = plt.figure(1)\n");
	issue_command(cmdstr);

	sprintf(cmdstr, "ax = plt.subplot(111)\n");
	issue_command(cmdstr);
}

PyHist::~PyHist(){
	sprintf(cmdstr, "plt.close()\n");
	issue_command(cmdstr);
	int rval = pclose(pypipe);
	assrt(rval != -1);
	
	//pipe must be closed (as above) before data is removed.
	if(savedata==0){
		sprintf(cmdstr, "rm FIGS/%s_*", name);
		system(cmdstr);
	}
	
	for(int i=0; i < MAX_NUM_PYPLT_CMDS; i++)
		delete[] cmd[i];
}

void PyHist::issue_command(const char *cstr){
	assrt(cmdnum < MAX_NUM_PYPLT_CMDS);
	int len = strlen(cstr);
	assrt(len < MAX_CMD_PYPLT_LEN-1);
	assrt(cstr[len-1] == '\n');
	
	strcpy(cmd[cmdnum], cstr);
	fprintf(pypipe, "%s", cmd[cmdnum++]);
}

void PyHist::prep_data(double *x, int n){
	char fname[200];
	sprintf(fname, "FIGS/%s_x.dat", name);
	array_out(x, n, 1, fname);
	sprintf(cmdstr, 
		"x = genfromtxt('%s_x.dat', dtype='float')\n", name);
	issue_command(cmdstr);
}

void PyHist::hist(double *x, int n){
	assrt(state == 0);
	state = 1;
	prep_data(x, n);
}

void PyHist::bins(int b){
	assrt(state == 1);
	state = 2;
	sprintf(cmdstr, 
		"h, b, p = plt.hist(x, bins = %d)\n", b);
	issue_command(cmdstr);
}

void PyHist::title(const char* s){
	assrt(state == 2);
	sprintf(cmdstr, "plt.title('%s', fontsize=20) \n", s);
	issue_command(cmdstr);
}

void PyHist::pycmd(const char* s){
	assrt(state = 2);
	int len = strlen(s);
	assrt(len < MAX_CMD_PYPLT_LEN-1);
	assrt(s[len-1] == '\n');
	issue_command(s);
}

void PyHist::show(){
	assrt(state == 2);
	state = 3;
	if(pipe_state == PLTON){
		sprintf(cmdstr, "plt.show() \n");
		issue_command(cmdstr);
	}
	else{
		sprintf(cmdstr, "plt.savefig('%s.pdf') \n",name);
		issue_command(cmdstr);
	}
}


void PyHist::savescript(){
	assrt(state==3);
	savedata = 1;
	char fname[200];
	sprintf(fname, "FIGS/%s.py", name);
	FILE *fp = fopen(fname, "w");
	fprintf(fp, "#! /usr/bin/env python\n");
	for(int i=0; i < cmdnum; i++)
		fprintf(fp, "%s", cmd[i]);
	fclose(fp);
}

