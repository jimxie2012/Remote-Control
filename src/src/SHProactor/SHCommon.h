#pragma once
#ifdef WIN32
   typedef unsigned int (*tc_thread_func)(void*);
#else
   typedef void * (*tc_thread_func)(void*);
#endif
#include <time.h>
#include <string.h>
#ifdef WIN32
#define SH_THREAD_RETURN unsigned int
#else
#define SH_THREAD_RETURN void *
#endif

using namespace std;

class CSHCommon
{
public:	
      static void           Sleep(unsigned int seconds);
      static string         GetExePath();
      static unsigned int   Atoul(const char *data);
      static unsigned int   GetSeconds(struct tm last,struct tm first );
      static string         GetTimeString();
      static string         GetTimeString(struct tm tmData);
      static struct tm      GetCurrentDateTime();	
      static bool           BeginThread(tc_thread_func funAddress, void * param );
};
