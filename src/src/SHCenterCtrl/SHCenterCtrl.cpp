#include "StdAfx.h"
#include "SHCenter.h"
#include <string>
#include <SHLibCommon.h>
#include "utilit.h"
#include <stdlib.h>
#include <string.h>
#include <SHConfig.h>
#include <stdio.h>
#include "SHDbAgent.h"

void RunProxy(){
   CSHConfig cfg("proxy");
   if (!cfg.Load()){
       printf("can not load proxy config file\n");
       return;
   }
   printf("proxy \n");
   printf("data port %d\n",cfg.m_dataPort);
   printf("pulse port %d:%d:%d\n",cfg.m_pulsePort1,cfg.m_pulsePort2,cfg.m_pulsePort3);
   printf("max aio %d\n",cfg.m_maxAio);
   printf("check timer %d\n",cfg.m_checkTimer);
   printf("time out %d\n",cfg.m_timeOut);
   printf("thread account %d\n",cfg.m_threadAccount);
   printf("log level = %d\n",cfg.m_logLevel);
   printf("sleep time %d\n",cfg.m_sleepTime);
   SH_ProactorInit(cfg.m_withLog,cfg.m_logLevel);
   if ( !SH_StartProxy(cfg.m_dataPort,
                       cfg.m_pulsePort1,
                       cfg.m_pulsePort2,
                       cfg.m_pulsePort3,
                       cfg.m_maxAio,
                       cfg.m_checkTimer,
                       cfg.m_timeOut,
                       cfg.m_sleepTime,
                       cfg.m_threadAccount)){
      printf("Start Proxy error\n");
      return;
   }
   SH_RunTimeLoop();
   //SH_BeginLoop();
}

int main(int argc, char* argv[]){    
   RunProxy();
   CSHDbAgent db;
   if ( !db.Connect( ) ){
      printf("Can not connect to database !\n");
      return 0;
   }
   
   while(1){
      if (!db.PingDb()){
         printf("ping db error retry after 10 seconds\n");
         CSHCommon::Sleep(10);
         continue;
      }
      
      map<SH_AGENT_ID,CSHDataPulse::ST_DATA> on;
      SH_GetOnLine(on);
      map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= on.begin();
      for(it=on.begin();it!=on.end();++it){
          printf("set pulse %ld\n",it->first);
          if ( it->first != 0 )
              db.SetPulse(it->second);
      }
      db.SetOffLine();
      db.DeleteTimeOutWebSession();
      CSHCommon::Sleep(60);
   }
   return 0;
}

