#include <stdio.h>
#include "SHRawServer.h"
#include "SHCommon.h"
#include "SHSessionData.h"

CSHRawServer::CSHRawServer(){
    m_lockTime = CSHCommon::GetCurrentDateTime();
}
CSHRawServer::~CSHRawServer(){
}
void CSHRawServer::OnAccept(SH_HANDLE s){
    if ( this->IsBusy() ){
	printf("CSHRawServe Locked maxTime =%d \n",m_maxLockTime);
	this->SetPeer(s,0);
	this->Connect(m_proxyHost,m_proxyPort,s);
  }
}
void CSHRawServer::OnConnect(SH_HANDLE s){
	if ( this->IsBusy() ){
	     CSHDataHead info(CMD_CONNECT);
             info.SetFlag( false );
             info.SetSession(m_agentId);
             info.SetProxy(m_proxyHost,m_proxyPort);
             info.SetRemote(m_remoteHost,m_remotePort);
	    
	     SetBusy(false);

	     printf("CSHRawServer Released %d proxy=%s:%d,remote=%s:%d\n",m_agentId,m_proxyHost,m_proxyPort,m_remoteHost,m_remotePort);
      
	     if ( this->Write(s,(char *)info.GetData(),CSHDataHead::GetSize() ) )
             this->Read(s,CSHDataHead::GetSize());
	}
}
void CSHRawServer::OnMessage(SH_HANDLE s,const char *buf,int size){
     SH_HANDLE peer     = this->GetPeer(s);
     SH_HANDLE peerPeer = this->GetPeer(peer);
     ///check if first response from proxy
     if ( peerPeer == 0 ){
         if ( size == CSHDataHead::GetSize() ){
             CSHDataHead info(buf);
             if ( info.GetFlag () ){
                this->SetPeer(peer,s);
                this->Read(peer,0);
                this->Read(s,0);  
             }
        }
        else{
             printf("wrong size from proxy %d\n",size);
        } 
     }
     else{
         if ( this->Write(peer,buf,size) ){
            this->Read(s,0);
         }
     }
}
void CSHRawServer::OnClose(SH_HANDLE s){
     SH_HANDLE peer = GetPeer(s);
     if ( peer != 0 ){
        SetPeer(peer,0);
        Close(peer);
     }
     SetPeer(s,0);
     CSHProactor::OnClose(s);
}
bool CSHRawServer::Start(int port,
		         int maxIO,
			 int checkTimer,
			 int timeOut,
			 int threadAccount,
			 const char *proxyHost,
			 int proxyPort,
                         int maxLockTime){
	if( strlen(proxyHost) >= sizeof(m_proxyHost)){
        printf("error proxy host name %s\n",m_proxyHost);
		return false;
	}
	m_maxLockTime=maxLockTime;
	m_port = port;
	strcpy(m_proxyHost,proxyHost);
	m_proxyPort = proxyPort;
	if (!this->StartClient(maxIO,checkTimer,timeOut,threadAccount)){
		printf("cannot start proactor client thead account %d\n",threadAccount);
		return false;
	}
	if (!this->StartServer(port,maxIO,checkTimer,timeOut,threadAccount)){
		printf("cannot start proactor server port = %d thread account %d\n",port,threadAccount);
		return false;
	}
	return true;
}
void CSHRawServer::Stop(){
    this->StopClient();
	this->StopServer();
}
void CSHRawServer::SetRemote(SH_HANDLE agentId,
		                     const char *remoteHost,
							 int remotePort,
							 int maxLockTime){
    m_lock.Lock();
    m_agentId = agentId;
    strcpy(m_remoteHost,remoteHost);
    m_remotePort = remotePort;
    m_maxLockTime = maxLockTime;
    m_lock.UnLock();
}
bool CSHRawServer::IsBusy(){
    m_lock.Lock();
    bool ret = m_isBusy;
    m_lock.UnLock();
    return ret;
}
void CSHRawServer::SetBusy(bool isBusy){
    m_lock.Lock();
    m_isBusy = isBusy;
    if ( m_isBusy )
       m_lockTime = CSHCommon::GetCurrentDateTime();
    m_lock.UnLock();
}	
int CSHRawServer::GetPort(){
    return m_port;
}
struct tm CSHRawServer::GetLockTime(){
	m_lock.Lock();
	struct tm ret = m_lockTime;
	m_lock.UnLock();
	return ret;
}
int CSHRawServer::GetMaxLockTime(){
	m_lock.Lock();
	int ret = m_maxLockTime;
	m_lock.UnLock();
	return ret;
}
