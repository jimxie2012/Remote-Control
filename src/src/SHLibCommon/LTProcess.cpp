#ifdef _LINUX
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/resource.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#define HZ 100
#define min(a,b) (a<b?a:b)
#define max(a,b) (a>b?a:b)
const void *memrchr(const void *s, int c, size_t n);
int g_pid = 0;
char g_logMsg[256] = {'\0'};

//extern "C" __declspec(dllexport)  int LTExeLimit(const char *,int);

inline long timediff(const struct timespec *ta,const struct timespec *tb) 
{
    unsigned long us = (ta->tv_sec-tb->tv_sec)*1000000 + (ta->tv_nsec/1000 - tb->tv_nsec/1000);
    return us;
}
int getpidof(const char *process) 
{
	if (setpriority(PRIO_PROCESS,getpid(),19)!=0) 
	{
		strcpy(g_logMsg,"Warning: cannot renice\n");
	}
	char exelink[20];
	char exepath[PATH_MAX+1];
	int pid=0;
	int i=0;
	while(1)
	{
		DIR *dip;
		struct dirent *dit;
		if ((dip = opendir("/proc")) == NULL) 
		{
			strcpy(g_logMsg,"Error: opendir /proc \n");
			return -1;
		}
		while ((dit = readdir(dip)) != NULL) 
		{
			pid=atoi(dit->d_name);
			if (pid>0) {
				sprintf(exelink,"/proc/%d/exe",pid);
				int size=readlink(exelink,exepath,sizeof(exepath));
				if (size>0) {
					int found=0;
					if (process[0]=='/' && strncmp(exepath,process,size)==0 && size==strlen(process)) 
					{
							found=1;
					}
					else
					 {
							if (strncmp(exepath+size-strlen(process),process,strlen(process))==0) 
							{
								found=1;
							}
					}
					if (found==1)
					{
						if (kill(pid,SIGSTOP)==0 &&  kill(pid,SIGCONT)==0) 
						{
						  	goto done;
						}
						else
						{
							sprintf(g_logMsg,"Error: Process %d detected, but you don't have permission to control it\n",pid);
						}
					}
				}
			}
		}
		if (closedir(dip) == -1)
		{
			strcpy(g_logMsg,"Error: closedir /proc \n");
			return -1;
		}
		if (i++==0) 
		{
				sprintf(g_logMsg,"Warning: no target process found. Waiting for it...\n");
		}
		sleep(2);
	}
done:
	sprintf(g_logMsg,"Process %d detected\n",pid);
	if (setpriority(PRIO_PROCESS,getpid(),-20)!=0) 
	{
		sprintf(g_logMsg,"Warning: cannot renice.\nTo work better you should run this program as root.\n");
	}
	return pid;
}
void quit(int sig) 
{
	kill(g_pid,SIGCONT);
	strcpy(g_logMsg,"Exiting...\n");
	exit(0);
}
//get jiffies count from /proc filesystem
int getjiffies(int pid) 
{
	static char stat[20];
	static char buffer[1024];
	sprintf(stat,"/proc/%d/stat",pid);
	FILE *f=fopen(stat,"r");
	if (f==NULL) return -1;
	fgets(buffer,sizeof(buffer),f);
	fclose(f);
	char *p=buffer;
	p=(char*)memchr(p+1,')',sizeof(buffer)-(p-buffer));
	int sp=12;
	while (sp--)
		p=(char *)memchr(p+1,' ',sizeof(buffer)-(p-buffer));
	//user mode jiffies
	int utime=atoi(p+1);
	p=(char *)memchr(p+1,' ',sizeof(buffer)-(p-buffer));
	//kernel mode jiffies
	int ktime=atoi(p+1);
	return utime+ktime;
}
struct process_screenshot
{
	struct timespec when;	//timestamp
	int jiffies;	//jiffies count of the process
	int cputime;	//microseconds of work from previous screenshot to current
};
struct cpu_usage {
	float pcpu;
	float workingrate;
};
//this function is an autonomous dynamic system
//it works with static variables (state variables of the system), that keep memory of recent past
//its aim is to estimate the cpu usage of the process
//to work properly it should be called in a fixed periodic way
//perhaps i will put it in a separate thread...
int compute_cpu_usage(int pid,int last_working_quantum,struct cpu_usage *pusage)
{
	#define MEM_ORDER 10
	//circular buffer containing last MEM_ORDER process screenshots
	static struct process_screenshot ps[MEM_ORDER];
	//the last screenshot recorded in the buffer
	static int front=-1;
	//the oldest screenshot recorded in the buffer
	static int tail=0;
	if (pusage==NULL) {
		//reinit static variables
		front=-1;
		tail=0;
		return 0;
	}
	//let's advance front index and save the screenshot
	front=(front+1)%MEM_ORDER;
	int j=getjiffies(pid);
	if (j>=0) ps[front].jiffies=j;
	else return -1;	//error: pid does not exist
	clock_gettime(CLOCK_REALTIME,&(ps[front].when));
	ps[front].cputime=last_working_quantum;
	//buffer actual size is: (front-tail+MEM_ORDER)%MEM_ORDER+1
	int size=(front-tail+MEM_ORDER)%MEM_ORDER+1;
	if (size==1) {
		//not enough samples taken (it's the first one!), return -1
		pusage->pcpu=-1;
		pusage->workingrate=1;
		return 0;
	}
	else {
		//now we can calculate cpu usage, interval dt and dtwork are expressed in microseconds
		long dt=timediff(&(ps[front].when),&(ps[tail].when));
		long dtwork=0;
		int i=(tail+1)%MEM_ORDER;
		int max=(front+1)%MEM_ORDER;
		do {
			dtwork+=ps[i].cputime;
			i=(i+1)%MEM_ORDER;
		} while (i!=max);
		int used=ps[front].jiffies-ps[tail].jiffies;
		float usage=(used*1000000.0/HZ)/dtwork;
		pusage->workingrate=1.0*dtwork/dt;
		pusage->pcpu=usage*pusage->workingrate;
		if (size==MEM_ORDER)
			tail=(tail+1)%MEM_ORDER;
		return 0;
	}
	#undef MEM_ORDER
}
int LTPidGet(const char *exe)
{
	 return getpidof(exe);
}
int LTPidLimit(int pid , int percent)
{
	g_pid = pid;
  int perclimit=percent;
	signal(SIGINT,quit);
	signal(SIGTERM,quit);
	int period=100000;
	struct timespec twork,tsleep;   
	memset(&twork,0,sizeof(struct timespec));
	memset(&tsleep,0,sizeof(struct timespec));
	compute_cpu_usage(0,0,NULL);
	int i=0;
	struct timespec startwork,endwork;
	long workingtime=0;
	float pcpu_avg=0;
	float limit=perclimit/100.0;
	while(1) 
	{
		struct cpu_usage cu;
		if (compute_cpu_usage(pid,workingtime,&cu)==-1) 
		{
			sprintf(g_logMsg,"Process %d dead!\n",pid);
			return -1;		
		}
		float pcpu=cu.pcpu;
		float workingrate=cu.workingrate;
		if (pcpu>0)
		{
			twork.tv_nsec=min(period*limit*1000/pcpu*workingrate,period*1000);
		}
		else if (pcpu==0) {
			twork.tv_nsec=period*1000;		}
		else if (pcpu==-1) {
			pcpu=limit;
			workingrate=limit;
			twork.tv_nsec=min(period*limit*1000,period*1000);
		}
		tsleep.tv_nsec=period*1000-twork.tv_nsec;
		pcpu_avg=(pcpu_avg*i+pcpu)/(i+1);
		if (limit<1 && limit>0) 
		{
			if (kill(pid,SIGCONT)!=0) 
			{
				sprintf(g_logMsg,"Process %d dead!\n",pid);
				return -2;
			}
		}
		clock_gettime(CLOCK_REALTIME,&startwork);
		nanosleep(&twork,NULL);		
		clock_gettime(CLOCK_REALTIME,&endwork);
		workingtime=timediff(&endwork,&startwork);
		if (limit<1) 
		{
			if (kill(pid,SIGSTOP)!=0) 
			{
				sprintf(g_logMsg,"Process %d dead!\n",pid);
				return -2;
			}
			nanosleep(&tsleep,NULL);	//now process is sleeping
		}
		i++;
	 }
	 return 0;
}
//extern "C" __declspec(dllexport)  int LTExeLimit(const char *,int); 
int LTExeLimit(const char *exe, int percent)
{
	 int pid= getpidof(exe);
	 if ( pid <= 0 )
	 {
	 	  sprintf(g_logMsg,"GetPid error : %s \n",exe);
	    return -1;
	 }
	 return LTPidLimit(pid,percent);
}
int LTLimitTest()
{
	printf("LTExeLimit(a.out,1)");
	return LTExeLimit("a.out",1);
}
#endif
