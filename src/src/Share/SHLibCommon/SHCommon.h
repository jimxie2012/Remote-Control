#pragma once
#include "Common.h"
#ifdef WIN32
   typedef unsigned int (*tc_thread_func)(void*);
#else
   typedef void * (*tc_thread_func)(void*);
#endif
#include <time.h>
#define SH_LOG_NONE   0
#define SH_LOG_DEBUG  1
#define SH_LOG_INFO   2
#define SH_LOG_ERROR  3
#define SH_LOG_CRIT   4
class SH_EXPORT CSHCommon
{
public:	
	  static bool           AddLog(int level,string log,...);
      static bool           ReadConfig(const char *fileName,const char *szSection, const char *szKey,char *data);
      static bool           WriteConfig(const char *fileName,const char *szSection, const char *szKey,char *data);
      static void           Sleep(unsigned int seconds);
      static string         GetExePath();
      static unsigned int   Atoul(const char *data);
      static unsigned int   GetSeconds(struct tm last,struct tm first );
      static string         GetTimeString();
      static string         GetTimeString(struct tm tmData);
      static struct tm      GetCurrentDateTime();	
      static bool           BeginThread(tc_thread_func funAddress, void * param );
#ifdef _LINUX
      static bool           CpuLimit(const char *exe, int percent);
#endif
};
