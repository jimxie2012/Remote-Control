#include "stdio.h"
#include <string>
#include <SHLibCommon.h>
#include "utilit.h"
#include <stdlib.h>
#include <string.h>
#include <SHConfig.h>

void extern daemonize(void);

void RunClient(){
   CSHConfig cfg("client");
   if (!cfg.Load()){
       printf("can not load client file\n");
       return;
   }
   printf("max aio %d\n",cfg.m_maxAio);
   printf("check timer %d\n",cfg.m_checkTimer);
   printf("time out %d\n",cfg.m_timeOut);
   printf("thread account %d\n",cfg.m_threadAccount);
   printf("Agent ID %u\n",cfg.m_agentId);
   printf("Agent pass %s\n",cfg.m_agentAcl.c_str());
   printf("Proxy Host %s\n",cfg.m_proxyHost.c_str());
   printf("pulse port %d:%d:%d\n",cfg.m_pulsePort1,cfg.m_pulsePort2,cfg.m_pulsePort3);
   printf("sleep time %d\n",cfg.m_sleepTime);

   if (SH_ProactorInit(cfg.m_withLog,cfg.m_logLevel) == NULL ){
	   printf("init error");
   }
   if( !SH_StartClient(cfg.m_maxAio,
                       cfg.m_checkTimer,
                       cfg.m_timeOut,
                       cfg.m_threadAccount,
                       cfg.m_agentId,
                       cfg.m_agentAcl.c_str(),
                       cfg.m_proxyHost.c_str(),
                       cfg.m_pulsePort1,
                       cfg.m_pulsePort2,
                       cfg.m_pulsePort3,
                       cfg.m_sleepTime)){
       printf("strart client error\n");
       return ;
   }
#ifdef _LINUX
   SH_BeginLoop();
#endif
}
void RunServer(){
   CSHConfig cfg("server");
   if (!cfg.Load()){
       printf("can not load server file\n");
       return;
   }
   printf("max aio %d\n",cfg.m_maxAio);
   printf("check timer %d\n",cfg.m_checkTimer);
   printf("time out %d\n",cfg.m_timeOut);
   printf("thread account %d\n",cfg.m_threadAccount);
   printf("Agent ID %u\n",cfg.m_agentId);
   printf("Agent acl %s\n",cfg.m_agentAcl.c_str());
   printf("Proxy Host %s\n",cfg.m_proxyHost.c_str());
   printf("dataPort %u\n",cfg.m_dataPort);

   int vServerList[1024] = { 0 };
   int vServerAccount = 0;
   for ( int i = 0 ; i < cfg.m_vServerList.size() ; i ++ ){
       vServerList[vServerAccount] = cfg.m_vServerList.at(i);
       printf("listen port %d\n",vServerList[vServerAccount]);
       vServerAccount = vServerAccount + 1;
   }
   SH_ProactorInit(cfg.m_withLog,cfg.m_logLevel);
   if ( !SH_StartServer(cfg.m_maxAio,
                        cfg.m_checkTimer,
                        cfg.m_timeOut,
                        cfg.m_threadAccount,
                        cfg.m_agentId,
                        cfg.m_agentAcl.c_str(),
                        cfg.m_proxyHost.c_str(),
                        cfg.m_dataPort,
                        cfg.m_vServerPort,
                        vServerList,
                        vServerAccount,
                        cfg.m_maxLockTime)){
        printf("start server error\n");
        return;
   }
   SH_BeginLoop();
}
void RunAlloc(string cmdStr){
    string     vServer;
    int        vPort;
    SH_HANDLE  agentId;
    string     remoteHost;
    int        remotePort;
    int        lockTime;
    string     user;
    string     pass;
    string     tmp;
    CSHString::GetStrValue(cmdStr,"vserver",vServer,"#");
    CSHString::GetStrValue(cmdStr,"vport",tmp,"#");
    vPort = atoi(tmp.c_str());
    CSHString::GetStrValue(cmdStr,"agentid",tmp,"#");
    agentId = atoi(tmp.c_str());
    CSHString::GetStrValue(cmdStr,"remotehost",remoteHost,"#");
    CSHString::GetStrValue(cmdStr,"remoteport",tmp,"#");
    remotePort = atoi(tmp.c_str());
    CSHString::GetStrValue(cmdStr,"locktime",tmp,"#");
    lockTime = atoi(tmp.c_str());
    CSHString::GetStrValue(cmdStr,"user",user,"#");
    CSHString::GetStrValue(cmdStr,"pass",pass,"#");
    int port =  SH_AllocPort(vServer.c_str(),vPort,agentId,remoteHost.c_str(),remotePort,lockTime,user.c_str(),pass.c_str());
    printf("alloc port is %d \n",port);
}
#ifdef _LINUX
int main(int argc, char *argv[]){
	if ( argc <= 1 ){
            printf("error command line \n");
	    return 0;
	}
	string role = argv[1];
	printf("runnint role %s\n",role.c_str());
        if ( role == KIND_SERVER){
            daemonize();
            RunServer();
        }
        if ( role == KIND_CLIENT){
            daemonize();
            RunClient();
        }
        if ( role == KIND_ALLOC ){
           if ( argc <= 2 ){
              printf("error alloc command line \n");
              return 0;
           }
           RunAlloc(argv[2]);
        }
	return 0;
}
#endif
