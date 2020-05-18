#include "SHProcess.h"
#ifdef WIN32
#include <process.h>
#include <conio.h>
#endif
#include "ace/Thread_Manager.h"
#include "ace/Process_Manager.h"
#include <sys/stat.h> 
#ifdef _LINUX
#include <unistd.h>
#include <stdlib.h>
#endif
#include <vector>
#include <map>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "ace/String_Base.h"
#include "ace/streams.h"
#include "ace/Log_Msg.h"
#include "ace/SStringfwd.h"
#include "TPCommon.h"
#include "SHCommon.h"
#include<iostream>
#include<string.h>
using namespace std;

int  CSHProcess::OpenProgress(const char *cmdLine)
{
    ACE_Process_Manager *processManager = ACE_Process_Manager::instance();
	int ret = 0;
	char path[512] = { 0 };
	string folder = CSHCommon::GetExePath();
	ACE_Process_Options options; 
    options.handle_inheritance(0);     
    options.working_directory(folder.c_str());    
	options.command_line (ACE_TEXT (cmdLine));
//    options.avoid_zombies(1);
#ifdef _LINUX
        signal(SIGCLD, SIG_IGN);
#endif
	pid_t processID=processManager->spawn(options);

	if (processID==ACE_INVALID_PID)
	{
		ret = 0;
	}
	else
	{
	   ret = processID;
	}
	printf("process id =%u\n",ret);
	return processID;
}
bool CSHProcess::KillProgress(int pid)
{
    ACE_Process_Manager *processManager = ACE_Process_Manager::instance();
	bool ret = false;
    if ( processManager->terminate(pid)==0 )
    {
       ret = true;
    }
#ifdef _LINUX
     char cmd[1024] = { 0 };
     sprintf(cmd,"kill -9 %u\r\n",pid);
     system(cmd);
#endif
	return ret;
}
