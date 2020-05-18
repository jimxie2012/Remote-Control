#include <stdio.h>
#include <list>
#include "SHUdpServer.h"
#include "SHCommon.h"

CSHUdpServer::CSHUdpServer(void){
	m_list = new list<CSHRawServer *>;
	m_list->clear();
}
CSHUdpServer::~CSHUdpServer(void){
	ClearAio();
	delete m_list;
}
CSHRawServer * CSHUdpServer::FindFreeAio(){
	CSHRawServer *ret = NULL;
	m_lock.Lock();
    list<CSHRawServer *>::iterator it;
    for(it=m_list->begin();it!=m_list->end();++it){
        CSHRawServer *tmp = *it;
		struct tm  now  = CSHCommon::GetCurrentDateTime();
		struct tm  last = tmp->GetLockTime();
		int seconds = CSHCommon::GetSeconds(now,last);
		printf("lockTime = %d maxLockTime=%d\n",seconds,tmp->GetMaxLockTime());
		if ( tmp->IsBusy() ){
			if ( seconds > tmp->GetMaxLockTime() ){
				tmp->SetBusy( false );
			}
		}
		if ( !tmp->IsBusy() ){
			ret = tmp;
			break;
		}
    }
	m_lock.UnLock();
	return ret;
}
void CSHUdpServer::ClearAio(){	
	m_isWorking = false;
	m_lock.Lock();
    list<CSHRawServer *>::iterator it;
    for(it=m_list->begin();it!=m_list->end();++it){
        CSHRawServer *tmp = *it;
		delete tmp;
    }
	m_list->clear();
	m_lock.UnLock();
}
bool CSHUdpServer::AppendAio(int port,
	                     int maxIO,
			     int checkTimer,
			     int timeOut,
			     int threadAccount,
			     const char *proxyHost,
			     int proxyPort,
                             int maxLockTime){
	m_lock.Lock();

	CSHRawServer *aio = new CSHRawServer();
	bool ret = aio->Start(port,maxIO,checkTimer,timeOut,threadAccount,proxyHost,proxyPort,maxLockTime);
	aio->SetBusy(false);
	if ( ret )
		m_list->push_back(aio);
	else
		aio->Stop();
	
	m_lock.UnLock();
	return ret;
}
void CSHUdpServer::Stop(){
	m_isWorking = false;
	m_udp.Close();
	ClearAio();
}
bool CSHUdpServer::StartUdp(int port){
	if ( !m_udp.Create() ){
	   printf("creat udp server error %d\n",port);
	   return false;
	}
	if ( !m_udp.Bind(port,"0.0.0.0") ){
	   printf("bind udp server error %d\n",port);
	   return false;
	}
    m_isWorking = true;
	if ( !CSHCommon::BeginThread(UdpThread,this)){
	   printf("start udp  server thread error\n");
           m_isWorking = false;
	   return false;
	}
	printf("udp raw server started\n");
    return true;
}
SH_THREAD_RETURN CSHUdpServer::UdpThread(void *param){
     CSHUdpServer *server = ( CSHUdpServer * )param;
     CSHDataConnect info;
     CSHLock lock;
     while( server->m_isWorking ){
          if ( server->m_udp.ReadConnect ( info ) ){
                        lock.Lock();
	  		printf("udp connect required %d(%s:%d)(%s,%s),maxLockTime(%d)\n",
				     info.GetAgentId(),
					 info.GetRemoteHost().c_str(),
					 info.GetRemotePort(),
					 info.GetUser().c_str(),
					 info.GetPass().c_str(),
					 info.GetMaxLockTime());

			  CSHRawServer *aio = server->FindFreeAio();
			  if ( aio == NULL ){
				  info.SetFlag(false);
				  info.SetResponsePort(0);
				  server->m_udp.SendConnect(info);
			  }
			  else{
				  aio->SetBusy( true );
				  aio->SetRemote(info.GetAgentId(),
					             info.GetRemoteHost().c_str(),
								 info.GetRemotePort(),
								 info.GetMaxLockTime());
                 
				  info.SetResponsePort( aio->GetPort() );
				  info.SetFlag(true);
				  server->m_udp.SendConnect(info);
			  }
                          lock.UnLock();
          }
    }
    return 0;
}
int CSHUdpServer::AllocPort(const char *rawHost,
		                    int rawPort,
							SH_HANDLE  agengId,
							const char *remoteHost,
							int remotePort,
							int maxLockTime,
							const char *userName,
							const char *password){
    int ret = 0;
    CSHUdpSocket udp;
    if ( !udp.Create() ){
         printf("creat udp client error\n");
	 return ret;
    }
    CSHAddr addr;
    addr.SetAddr(rawHost,rawPort);
    CSHDataConnect info;
    info.SetFlag(false);
    info.SetUser(maxLockTime,userName,password);
    info.SetRemote(agengId,remoteHost,remotePort);
    if ( udp.SendConnect(addr,info)){
	if ( udp.ReadConnect(info) ){
	     if ( info.GetFlag() ){
	        ret = info.GetResponsePort();
	     }
	}
    }
    udp.Close();
    return ret;
}
