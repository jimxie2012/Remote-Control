#include <stdio.h>
#include "SHRawClient.h"
#include "SHAddr.h"
#include "SHSessionData.h"

CSHRawClient::CSHRawClient(){
	m_pulse.m_rawClient = this;
    m_lock.Lock();
	m_map = new map<SH_HANDLE,CSHDataHead *>;
    m_map->clear();
    m_lock.UnLock();
}
CSHRawClient::~CSHRawClient(){
    m_lock.Lock();
    m_map->clear();
    m_lock.UnLock();
	delete m_map;
}
void CSHRawClient::OnAccept(SH_HANDLE s){
}
CSHDataHead * CSHRawClient::FindSession(SH_HANDLE s){
     m_lock.Lock();
     CSHDataHead *ret = NULL;
     map<SH_HANDLE,CSHDataHead *>::iterator it = m_map->find(s);
     if ( it != m_map->end() ){
          ret = it->second;
     }
     m_lock.UnLock();
     return ret;
}
CSHDataHead * CSHRawClient::UpdateSession(SH_HANDLE proxyHandle,SH_HANDLE localHandle){
     CSHDataHead *ret = NULL;
     m_lock.Lock();
     map<SH_HANDLE,CSHDataHead *>::iterator it = m_map->find(proxyHandle);
     if ( it != m_map->end() ){
          CSHDataHead *head = it->second;
          CSHDataHead *newHead = new CSHDataHead((const char *)head->GetData());
          delete head;
          m_map->erase(it);
          newHead->SetSession(localHandle);
          m_map->insert(make_pair(newHead->GetSession(),newHead));
          ret = newHead;
     }
     m_lock.UnLock();
     return ret;
}
void CSHRawClient::RemoveSession(SH_HANDLE s){
     m_lock.Lock();
     map<SH_HANDLE,CSHDataHead *>::iterator it = m_map->find(s);
     if ( it != m_map->end() ){
          CSHDataHead *data = it->second;
          delete data;
          m_map->erase(it);
     }
     m_lock.UnLock();
}
void CSHRawClient::AckUdp(CSHDataHead &info){
     RemoveSession(info.GetSession());
     m_lock.Lock();
     CSHDataHead *data = new  CSHDataHead((const char *)info.GetData());
	 SH_HANDLE proxyHandle = info.GetSession();
	 if ( data != NULL ){
		 m_map->insert(make_pair(data->GetSession(),data));
          if ( !this->Connect(info.GetProxyHost().c_str(),info.GetProxyPort(),info.GetSession())){
		      m_lock.UnLock();
		      RemoveSession(proxyHandle); 
		      return ;
	      }
	 }
     m_lock.UnLock();
}
void CSHRawClient::OnConnect(SH_HANDLE s){
     SH_HANDLE  peer     = this->GetPeer(s);
     CSHAioAddr peerAddr = this->GetPeerAddr(s);
     CSHAddr addr1,addr2;
     addr1.SetAddr(peerAddr.m_ip,peerAddr.m_port);

     CSHDataHead *info = this->FindSession(peer);
     if ( info == NULL ){
        printf("can not find session \n");
		return;
     }
     addr2.SetAddr(info->GetProxyHost().c_str(),info->GetProxyPort());
     if ( (addr1.GetIp() == addr2.GetIp() )&& (addr1.GetPort() == addr2.GetPort() ) ){	
        info = this->UpdateSession(peer,s) ;
        if ( info !=NULL )
	        if ( !this->Connect(info->GetRemoteHost().c_str(),info->GetRemotePort(),s)){
				//this->RemoveSession(peer);
			}
        //else
        //    this->RemoveSession(peer);
     }
     else{ 
        info->SetFlag( true );
        info->SetSession(this->GetPeer(peer));
        info->SetCmd(CMD_CONNECT_ACK);
        if ( this->Write(peer,(const char *)info->GetData(),CSHDataHead::GetSize())) {
             this->SetPeer(peer,s);
             this->Read(s,0 ) ;
             this->Read(peer,0 );
		}
		this->RemoveSession(info->GetSession());
    }
}
void CSHRawClient::OnMessage(SH_HANDLE s,const char *buf,int size){
     SH_HANDLE peer     = this->GetPeer(s);
     if( this->Write(peer,buf,size) ){
         this->Read(s,0);
     }
}
void CSHRawClient::OnClose(SH_HANDLE s){
     SH_HANDLE peer = GetPeer(s);
	 RemoveSession(s);
     if ( peer != 0 ){
	    RemoveSession(peer);
        SetPeer(peer,0);
        Close(peer);
     }
     SetPeer(s,0);
     CSHProactor::OnClose(s);
}
bool CSHRawClient::Start(int maxIO,
			 int checkTimer,
		         int timeOut,
			 int threadAccount,
			 SH_AGENT_ID agentId,
                         const char *passWord,
                         const char *proxyHost,
			 int pulsePort1,
                         int pulsePort2,
                         int pulsePort3,
                         int sleepTime){
	 if( !m_pulse.Create(agentId,passWord,proxyHost,pulsePort1,pulsePort2,pulsePort3,sleepTime)){
		 printf("create pulse object error\n");
	     return false;
	 }
  	 return StartClient(maxIO,checkTimer,timeOut,threadAccount);
}
void CSHRawClient::Stop(){
     m_pulse.ClosePulse();
	 this->StopClient();
}
