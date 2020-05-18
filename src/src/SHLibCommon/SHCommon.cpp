#include <stdio.h>
#include "SHCommon.h"
#include "ace/Thread_Manager.h"
#include "ace/Process_Manager.h"
#include <sys/stat.h> 
#ifdef _LINUX
#include <unistd.h>
#include <stdlib.h>
#endif
#include <sys/stat.h>

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
#include "ACEConfig.h"
#include<iostream>
#include<string.h>
#include "SHString.h"
#include "SHFile.h"
#include "SHLock.h"
#include <stdarg.h>
using namespace std;

string CSHCommon::GetExePath()
{
	string ret = "";	
	#ifdef WIN32
	   char process[1024] = { 0 };
		GetModuleFileName(NULL, process, MAX_PATH);
	    for ( int i = strlen(process);i>0 ;i --)
		{
			if ( process[i] =='\\')
			{
				process[i+1]='\0';
		     	break;
			}
		}
		ret = process;	
	#endif
	#ifdef _LINUX
		char buf[1024] = { 0 };
	   int count = readlink("/proc/self/exe",buf,sizeof(buf));
	   if (( count <= 0) || ( count > sizeof(buf)) )
	    {
		   return "";
	    }
	   for(int i = count ; i >= 0 ; i -- )
	    {
		   if ( buf[i] == '/' )
	        {
			   buf[i] = '\0';
			   break;
	        }
	    }
	   buf[count] = '\0';
	   ret = buf;
	   ret = ret + "/";
    #endif
	return ret;
}
bool CSHCommon::ReadConfig(const char *fileName,const char *szSection, const char *szKey,char *data)
{
	bool ret = false;	
	CConfigTool config;
	if ( config.open(fileName) == 0 ){
		ACE_TString strValue;
		if ( 0 == config.GetString(szSection,szKey, strValue) ){
                     strcpy(data, strValue.c_str());
                        string tmp = data;
                        string temp = CSHString::Trim(tmp);
                        strcpy(data, temp.c_str()); 
                        ret = true;
		}
		config.close();
	}
	return ret;
}
bool CSHCommon::WriteConfig(const char *fileName,const char *szSection, const char *szKey,char *data)
{
	bool ret = false;	
	CConfigTool config;
	if ( config.open(fileName) == 0 )
	{
		ACE_TString strValue=data;
		if ( 0 == config.SetString(szSection,szKey, strValue) )
		{
			ret = true;
		}
		config.close();
	}
	return ret;
}

void CSHCommon::Sleep(unsigned int seconds)
{
	ACE_OS::sleep(seconds);
}


bool CSHCommon::BeginThread(tc_thread_func funAddress, void * param)
{
	/*
	#ifdef WIN32
		while (  _beginthreadex(0,0,(unsigned int (__stdcall *)(void *))funAddress,param,0,0) == NULL )
		{
			CSHCommon::Sleep(1);
		} 
	#endif
	
	#ifdef _LINUX		
	    pthread_t thread_test;
 	    pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_CREATE_DETACHED );
        while ( pthread_create(&thread_test,&attr,(tc_thread_func)funAddress,param) != 0 )
	    {
	      CSHCommon::Sleep(1);
	    }
   #endif
   */
   if(ACE_Thread::spawn((ACE_THR_FUNC)funAddress,param,THR_DETACHED|THR_NEW_LWP)==-1)
	   return false;
   return true;
}
struct tm CSHCommon::GetCurrentDateTime()
{
   time_t tval;
   struct tm *now;
   tval= time(NULL);
   now = localtime(&tval);
   return *now;
}
unsigned int CSHCommon::GetSeconds(struct tm last, struct tm first)
{
   unsigned int seconds = (last.tm_year-first.tm_year)*365*24*3600+(last.tm_mon-first.tm_mon)*30*24*3600+(last.tm_mday-first.tm_mday)*24*3600+(last.tm_hour-first.tm_hour)*3600+(last.tm_min-first.tm_min)*60+(last.tm_sec-first.tm_sec);
   return seconds;
}

unsigned int CSHCommon::Atoul(const char *data)
{
   if (data == NULL )
      return 0;
   if (strcmp(data,"") == 0 )
      return 0;
   return   strtoul(data,NULL,10);
}
string  CSHCommon::GetTimeString(struct tm tmData)
{
	string ret;
	struct tm *now= &tmData;
	char nowTime[100] = { 0 };
	sprintf(nowTime,"%04d-%02d-%02d %02d:%02d:%02d",1900+now->tm_year,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
	ret = nowTime;
	return ret;
}
string  CSHCommon::GetTimeString()
{
	string ret;
	time_t tval;
	struct tm *now;
	tval= time(NULL);
	now= localtime(&tval);
	char nowTime[100] = { 0 };
	sprintf(nowTime,"%04d-%02d-%02d %02d:%02d:%02d",1900+now->tm_year,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
	ret = nowTime;
	return ret;
}
#ifdef _LINUX
int LTExeLimit(const char *exe, int percent);
bool CSHCommon::CpuLimit(const char *exe, int percent)
{
    bool ret = false;
    if ( LTExeLimit(exe,percent) == 0)
         ret = true;
    return false; 
}
#endif
bool CSHCommon::AddLog(int level,string log,...){
	if ( level == SH_LOG_NONE )
		return true;
	string log1;    
	va_list arglist;
    va_start( arglist, log );
	CSHString::strFormatEx(log1,log,arglist);
    va_end( arglist );
	char buf[4096] = { 0 };
	char tmp[4096] = { 0 };
	char levelstr[20] = { 0 };
	switch ( level ){
	case SH_LOG_NONE:
		strcpy(levelstr,"NONE");
		break;
	case SH_LOG_DEBUG:
		strcpy(levelstr,"DEBUG");
		break;	
	case SH_LOG_INFO:
		strcpy(levelstr,"INFO");
		break;	
	case SH_LOG_ERROR:
		strcpy(levelstr,"ERROR");
		break;	
	case SH_LOG_CRIT:
		strcpy(levelstr,"CRIT");
		break;
	default:
		break;
	}
        sprintf(buf,"[%s][%s]%s\n",CSHCommon::GetTimeString().c_str(),levelstr,log1.c_str());
        bool ret = true,ret1=false;
	static CSHLock lock;
	lock.Lock();
	CSHFile io;
	try{
	    if (!io.Open(CSHCommon::GetExePath()+"trace.log",false)){
	        ret = false;
            }
            else{
	        io.Seek(true);
	        io.WriteLn(buf);
                ret1 = true; 
	    }
        }
	catch(...){
	}
        
	try{
            if ( ret ) {
	        io.Close();
            }
	}
	catch(...){
	}
	lock.UnLock();
	return ret && ret1;
}
