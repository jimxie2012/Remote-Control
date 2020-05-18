#include <stdio.h>
#include "SHUdpPulse.h"
#include "SHRawClient.h"
#include "SHCommon.h"
#include "SHProcess.h"
#include "SHString.h"
bool SH_GetWithLog();

CSHUdpPulse::CSHUdpPulse(void) {
	m_isPulseing = false;
}
CSHUdpPulse::~CSHUdpPulse(void){
}
bool CSHUdpPulse::Create(SH_AGENT_ID agentId,
                         const char *acl,
                         const char *proxyHost,
                         int proxyPort1,
                         int proxyPort2,
                         int proxyPort3,
                         int sleepTime){
    if ( !CSHUdpSocket::Create() ){
        if ( SH_GetWithLog () ){
			CSHCommon::AddLog(SH_LOG_DEBUG,"create pulse socket error\n");
		}
		return false;
    }
    m_currentPulsePort = 0;
    m_proxyHost = proxyHost;
    m_proxyPort1= proxyPort1;
    m_proxyPort2= proxyPort2;
    m_proxyPort3= proxyPort3;
    if (sleepTime <=5)
        sleepTime = 5;
   
    CSHUdpSocket::SetAgent(agentId,acl);
    CSHUdpSocket::SetSleepTime(sleepTime);	
    m_isPulseing = true;
    if ( !CSHCommon::BeginThread(PaulseThread,this) ){
	    printf("start pulse thread error\n");
	    return false;
    }
    return true;
}
bool CSHUdpPulse::SendWithResponse(string proxy_ip,int proxy_port){
    struct tm dt_first = CSHCommon::GetCurrentDateTime();
    this->SetProxy(proxy_ip.c_str(),proxy_port);
    CSHAddr addr;
    addr.SetAddr(proxy_ip,proxy_port);
    //this->Connect(addr);
    if (!this->SendPulse())
        return false;

    while ( m_currentPulsePort == 0 ) {
        CSHCommon::Sleep(1);
        unsigned int seconds = CSHCommon::GetSeconds(CSHCommon::GetCurrentDateTime(),dt_first);
        if ( seconds >= 60 )
            return false;
    }
    return true;
}
bool CSHUdpPulse::SendAllPulse(){
	CSHAddr addr;	
	if ( m_currentPulsePort == 0 ){
		addr.SetAddr(m_proxyHost,m_proxyPort1);
		if ( CSHUdpSocket::IsServerAlive(addr)){
			m_currentPulsePort = m_proxyPort1;
			this->SetProxy(m_proxyHost.c_str(),m_currentPulsePort);
            return this->SendPulse();
		}
		addr.SetAddr(m_proxyHost,m_proxyPort2);
		if ( CSHUdpSocket::IsServerAlive(addr)){
			m_currentPulsePort = m_proxyPort2;
			this->SetProxy(m_proxyHost.c_str(),m_currentPulsePort);
            return this->SendPulse();
		}
		addr.SetAddr(m_proxyHost,m_proxyPort3);
		if ( CSHUdpSocket::IsServerAlive(addr)){
			m_currentPulsePort = m_proxyPort3;
			this->SetProxy(m_proxyHost.c_str(),m_currentPulsePort);
            return this->SendPulse();
		}
        return false;
    }
    else{
        this->SetProxy(m_proxyHost.c_str(),m_currentPulsePort);
        return this->SendPulse();
    }
	return true;
}
SH_THREAD_RETURN CSHUdpPulse::PaulseThread(void *param){	
   CSHUdpPulse  &paulse = *(( CSHUdpPulse *)param); 
   bool isFirst = true;
   paulse.m_currentPulsePort = 0 ;
   while( paulse.m_isPulseing ){
       if ( isFirst ){
           if ( !CSHCommon::BeginThread(ListenThread,param ) ){
			   if ( SH_GetWithLog () ){
			       CSHCommon::AddLog(SH_LOG_DEBUG,"begin listen thread error\n");
		       }
               return 0;
		   }
       }
       if (!paulse.SendAllPulse()){
		   //paulse.m_currentPulsePort = 0 ; 
		   if ( SH_GetWithLog () ){
			    CSHCommon::AddLog(SH_LOG_DEBUG,"send all pulse error\n");
		   }
	   }
	   int sleepTime = paulse.GetSleepTime();
	   if ( SH_GetWithLog () ){
		   CSHCommon::AddLog(SH_LOG_DEBUG,"begin sleep %d\n",sleepTime);
	   }
       isFirst = false;

       CSHCommon::Sleep( sleepTime );
   }
   return 0;
}
bool CheckLocalHost(const char *acl,const char *hostname){
	if ( strstr(acl,"N") != NULL )
	    return true;
	CSHAddr sa1;
	sa1.SetAddr(hostname);
	string localIp = CSHAddr::GetLocalIpAddress();
	vector<string> vec;
	CSHString::ConvertString2Vector(vec,localIp);
	for ( int i = 0 ; i < vec.size() ; i ++ ){
		CSHAddr sa2;
		sa2.SetAddr( vec.at(i).c_str());
		if (sa1.GetIp() == sa2.GetIp())
			return true;
	}
	if ( sa1.GetIpString() == "127.0.0.1")
		return true;
	return false;
}
SH_THREAD_RETURN CSHUdpPulse::ListenThread(void *param){
   CSHUdpPulse  &paulse = *(( CSHUdpPulse *)param);    
   CSHDataHead info(SH_NOOP);
   int lastKey = 0;  
   bool withLog = SH_GetWithLog();
   while ( paulse.m_isPulseing ){
	   try{
		   if ( !paulse.ReadConnect(info) ){
			   CSHCommon::Sleep(1); 
			   continue;
		   }
	   }
	   catch(...){
		   if ( withLog ){
				CSHCommon::AddLog(SH_LOG_INFO,"listen thread exception\n");
			}
		    CSHCommon::Sleep(1); 	
			continue;
	   }

	   if( info.GetCmd() == CMD_CONNECT ){		
		   	if ( withLog ){
				CSHCommon::AddLog(SH_LOG_INFO,"connect cmd = %d remoteHost  = %s,remotePost=%d\n",info.GetCmd(),info.GetRemoteHost().c_str(),info.GetRemotePort());
			}
		   if (( lastKey != 0 ) && ( lastKey == info.GetKey())){
			   if ( withLog ){
				   CSHCommon::AddLog(SH_LOG_ERROR,"auth key error,maybe duplicated packets\n");
		       }
	           continue;
	        }
		   lastKey = info.GetKey();
	       if ( CheckLocalHost(paulse.m_acl,info.GetRemoteHost().c_str() ) ){
			   paulse.m_rawClient->AckUdp(info);
	       }else{
		       if ( withLog ){
			      CSHCommon::AddLog(SH_LOG_ERROR,"permition  error,maybe duplicated packets\n");
			   }
		   }
		   continue;
	   }
       if( info.GetCmd() == SH_NOOP ){
           continue;
       }
	   /*
        if (info.GetCmd() == SH_SET_PORT){
			if ( withLog ){
				CSHCommon::AddLog(SH_LOG_INFO,"set port current =%d, to set = %d\n",paulse.m_currentPulsePort,paulse.GetPeer().GetPort());
			}
			g_lock_socket.Lock();
            if ( paulse.m_currentPulsePort == 0 ) {			
                paulse.m_currentPulsePort = paulse.GetPeer().GetPort();
            }
			g_lock_socket.UnLock();
            continue;
        }
		*/
		if ( withLog ){
			CSHCommon::AddLog(SH_LOG_ERROR,"wrong udp command %d\n",info.GetCmd());
	    }
   }
   return 0;
}
void CSHUdpPulse::ClosePulse(void){
   m_isPulseing = false;
   CSHUdpSocket::Close();
   CSHCommon::Sleep(5);
}
