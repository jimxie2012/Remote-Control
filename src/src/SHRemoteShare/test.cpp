#include <stdio.h>
#include "SHUdpServer.h"
#include "SHRawClient.h"
#include "SHRawProxy.h"
#include <SHString.h>

CSHRawProxy  g_proxy;
CSHRawClient g_client;
CSHUdpServer g_server;

extern "C"
{
int SH_EXPORT SH_AllocPort(const char *vServerIp,
                 int    vServerUdpPort,
                 SH_HANDLE agentId,
                 const char *remoteHost,
                 int remotePort,
                 int lockTime,
                 const char *userName,
                 const char *passwd){
    return  CSHUdpServer::AllocPort(vServerIp,vServerUdpPort,agentId,remoteHost,remotePort,lockTime,userName,passwd);
}
bool SH_EXPORT SH_StartProxy(int dataPort,
                   int pulsePort1,
                   int pulsePort2,
                   int pulsePort3,
                   int maxAio,
                   int checkTimer,
                   int timeOut,
                   int sleepTime,
                   int threadAccount){
    if(!g_proxy.Start(dataPort,pulsePort1,pulsePort2,pulsePort3,maxAio,checkTimer,timeOut,sleepTime,threadAccount)){
        g_proxy.Stop();
        printf("SH_StartProxy error\n");
        return false;
	}
    return true;
}
bool SH_EXPORT SH_StartClient(int maxAio,
                    int checkTimer,
                    int timeOut,
                    int threadAccount,
                    SH_HANDLE agentId,
                    const char *acl,
                    const char *proxyHost,
                    int   proxyPulsePort1,
                    int   proxyPulsePort2,
                    int   proxyPulsePort3,
                    int   sleepTime){
    if (!g_client.Start(maxAio,checkTimer,timeOut,threadAccount,agentId,acl,proxyHost,proxyPulsePort1,proxyPulsePort2,proxyPulsePort3,sleepTime)){
        g_client.Stop();
        printf("SH_StartClient error\n");
        return false;
    }
    return true;
}
bool SH_EXPORT SH_StartServer(int maxAio,
                    int checkTimer,
                    int timeOut,
                    int threadAccount,
                    SH_HANDLE agentId,
                    const char *agentAcl,
                    const char *proxyHost,
                    int   proxyDataPort,
                    int   udpPort,
                    int   tcpPortList[],
                    int   tcpPortAccount,
                    int maxLockTime){
    for ( int i = 0 ;i < tcpPortAccount ; i++ ){
         g_server.AppendAio(tcpPortList[i],maxAio,checkTimer,timeOut,threadAccount,proxyHost,proxyDataPort,maxLockTime);
    }
    if (!g_server.StartUdp(udpPort)){
         printf("SH_StartServer::Start udp error port = %d\r\n",udpPort);
         return false;;
    }
    return true;
}
bool SH_EXPORT SH_ProactorInit(bool enableLog,unsigned int logLevel){
    return CSHAio::Initialise(enableLog,logLevel);
}
int SH_EXPORT SH_BeginLoop(){
    while ( 1 )
        CSHAio::RunTimerLoop();
    return 0;
}
int SH_EXPORT SH_RunTimeLoop(){
    CSHAio::RunTimerLoop();
	return 0;
}
int SH_EXPORT SH_StopClient(){
    g_client.Stop();
    return 0;
}
int SH_GetOnLine_Apend(CSHUdpSocket *socket,map<SH_AGENT_ID,CSHDataPulse::ST_DATA> &OnLineList){
    map<SH_AGENT_ID,CSHDataPulse::ST_DATA> tmp;
    tmp.clear();
    socket->m_list.GetPulseList(tmp);
    map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= tmp.begin();
    for(it=tmp.begin();it!=tmp.end();++it){
         map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator t = OnLineList.find(it->first);
         if ( t == OnLineList.end()){
             OnLineList.insert(make_pair(it->first,it->second));
         }
         else{
             time_t t_current = mktime(&(it->second.m_activeTime)); 
             time_t t_last    = mktime(&(t->second.m_activeTime));
             if(t_current > t_last){
                 OnLineList.erase(t);
                 OnLineList.insert(make_pair(it->first,it->second));
             }
         }
    }
}
int SH_GetOnLine(map<SH_AGENT_ID,CSHDataPulse::ST_DATA> &OnLineList){
    OnLineList.clear();
    SH_GetOnLine_Apend(&(g_proxy.m_udp1),OnLineList);
    SH_GetOnLine_Apend(&(g_proxy.m_udp2),OnLineList);
    SH_GetOnLine_Apend(&(g_proxy.m_udp3),OnLineList);
    return 0;
}
}
bool SH_GetWithLog(){
   static bool is_first = false;
   static bool last_with_log = false;
   if (is_first)
	   return last_with_log;
   char temp[1024] = { 0 };
   string cfgFile = CSHCommon::GetExePath()+"shremote.cfg";
   bool withLog =false;
   if (CSHCommon::ReadConfig(cfgFile.c_str(),"client","withLog",temp)){
		string st0 = temp;
		string st1 = temp;
		string str2 = "yes";
		st1 = CSHString::Trim(st0);

		if ( CSHString::CmpStr(st1,str2) == 0 ){
			withLog = true;
		}
    }
   last_with_log = withLog;
   is_first = true;
   return withLog;
}
/*
#include "SHSessionData.h"
#include "SHDnsHead.h"
int sendUDP(char *buf,int size){
    int connfd, len = 0;
    struct  sockaddr_in servaddr;
    char    *p;
    int     i= sizeof(struct sockaddr_in);
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if ((connfd  =  socket(AF_INET, SOCK_DGRAM, 0 ))  <   0 ){
        perror( "socket error!\n " );
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(53);
    if(inet_pton(AF_INET,"222.184.9.243",&servaddr.sin_addr) < 0){
        perror("inet_pton error.\n");
        return -1;
    }
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if(sendto(connfd, buf, size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){                                                     
        perror("sendto error.\n");
        return -1;
    }
    char rBuf[1024];
    if((len = recvfrom(connfd, rBuf, sizeof(rBuf) , 0, (struct sockaddr *)&servaddr,(socklen_t*)(&i))) < 0){
          printf("timeout\n");
          return -1;
   }
   printf("recved %s\n",rBuf);
   close(connfd);
   return 0;
}
int main(){
	CSHDataPulse pulse;
	pulse.SetCmd(1);
	pulse.SetProxyAddr("remote.cnetbbs.com",1000);
	pulse.SetAgentAddr("agent.com",23);
	pulse.SetId(100,"123");
	pulse.SetPrivate("127.0.0.1","localhost");

	printf("GetPrivateIp %s\n",pulse.GetPrivateIp());
	printf("GetPrivateHost %s\n",pulse.GetPrivateHost());
	printf("GetPass %s\n",pulse.GetPass());
	printf("GetId %d\n",pulse.GetId());
	printf("GetAgentIp %s\n",pulse.GetAgentIp());
	printf("GetAgentPort %d\n",pulse.GetAgentPort());
	printf("GetCmd %d\n",pulse.GetCmd());
	printf("GetProxyHost %s\n",pulse.GetProxyHost());
	printf("GetProxyPort %d\n",pulse.GetProxyPort());
	printf("GetString %s\n",pulse.GetString().c_str());
	string tmp = pulse.GetString();
	CSHDnsHead dns;
	dns.EnRequest((char *)tmp.c_str());

	sendUDP(dns.m_buf,dns.m_size);
	getchar();
	return 0;
}
*/
