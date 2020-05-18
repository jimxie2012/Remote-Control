#include "SHAioSocket.h"
#include <stdio.h>
#include <SHAio.h>
#include <string>
#include <SHCommon.h>

CSHAioSocket::CSHAioSocket(){
    m_id = 0;
}
CSHAioSocket::~CSHAioSocket(){
}
int CSHAioSocket::GetNextId(){
    static CSHAioLock lock;
    static int id = 0;
    lock.Lock();
    id ++;
    if ( id > 65534 )
       id = 1 ;
    int ret = id;
    lock.UnLock();
    return ret;
}
void CSHAioSocket::OnCreate(){
    m_session->AddToMap(m_id,this);
    CSHAio::OnCreate();
    if ( m_isClient ) 
       m_session->OnConnect(m_id);
    else
       m_session->OnAccept(m_id);
}
void CSHAioSocket::Close(){
    //CSHAio::Close(); ///add by jim 2018-02-26
}
bool CSHAioSocket::Read(int size){
    return CSHAio::Read(size);
}
CSHAioAddr CSHAioSocket::GetPeerAddr(){
    return CSHAio::GetPeerAddr();
}
void CSHAioSocket::SetPeer(SH_HANDLE s){
    CSHAio::SetAct((const void *)s);
}
SH_HANDLE CSHAioSocket::GetPeer(){
    return ( long )(CSHAio::GetAct());
}
bool CSHAioSocket::Write(const char *buf,int size){
    return CSHAio::Write(buf,size);
}
void CSHAioSocket::OnMessage(const char *buf,int size){
    CSHAio::OnMessage(buf,size);
    m_session->OnMessage(m_id,buf,size);
}
void CSHAioSocket::OnWrite(){
    CSHAio::OnWrite();
}
void CSHAioSocket::OnClose(){
    m_session->OnClose(m_id);
    m_session->RemoveFromMap(m_id);
    CSHAio::OnClose();
}
void CSHAioSocket::OnTimeOut(){
    CSHAio::OnTimeOut();
}
CSHProactor::CSHProactor(){
	m_map = new map<SH_HANDLE,CSHAioSocket *>;
	m_map->clear();
}
CSHProactor::~CSHProactor(){
	m_map->clear();
	delete m_map;
}
void CSHProactor::AddToMap(SH_HANDLE s,CSHAioSocket *socket){
    m_lock.Lock();
    m_map->insert(make_pair(s,socket));
    m_lock.UnLock();
}
void CSHProactor::RemoveFromMap(SH_HANDLE s){
   m_lock.Lock();
   map<SH_HANDLE,CSHAioSocket *>::iterator it = m_map->find(s);
   if ( it != m_map->end() )
     m_map->erase(it); 
   m_lock.UnLock();
}
CSHAioSocket* CSHProactor::Find(SH_HANDLE s){
   CSHAioSocket *ret = NULL;
   m_lock.Lock();
   map<SH_HANDLE,CSHAioSocket *>::iterator it = m_map->find(s);
   if ( it != m_map->end() )
      ret = it->second;
   m_lock.UnLock();
   return ret;
}
bool CSHProactor::StartServer(int port,int maxIO,int checkTimer,int timeOut,int threadAccount){
    for ( int i = 0 ; i < maxIO ; i ++ ){
        CSHAioSocket *it = new CSHAioSocket();
        it->m_session = this;
        it->m_id = CSHAioSocket::GetNextId();
        it->m_isClient = false;
        m_server.m_list.Add(it);
    }
    return m_server.Start(port,checkTimer,timeOut,threadAccount);
}
void CSHProactor::StopServer(){
    m_server.Stop();
    list<CSHAio *>::iterator it;
	list<CSHAio *> *tList = m_server.m_list.GetList();
    for(it=tList->begin();it!=tList->end();++it){
        CSHAio *tmp = *it;
        delete ((CSHAioSocket *)tmp);
    }
    tList->clear();
}
bool CSHProactor::StartClient(int maxIO,int checkTimer,int timeOut,int threadAccount){
    for ( int i = 0 ; i < maxIO ; i ++ ){
        CSHAioSocket *it1 = new CSHAioSocket();
        it1->m_session = this;
        it1->m_id =CSHAioSocket::GetNextId();
        it1->m_isClient = true;
        m_client.m_list.Add(it1);
    }
    return m_client.Start(checkTimer,timeOut,threadAccount);
}
void CSHProactor::StopClient(){
    m_client.Stop();
    list<CSHAio *>::iterator it;
	list<CSHAio *> *tList = m_client.m_list.GetList();
    for(it=tList->begin();it!=tList->end();++it){
        CSHAio *tmp = *it;
        delete ((CSHAioSocket *)tmp);
    }   
    tList->clear();
}
void CSHProactor::OnConnect(SH_HANDLE s){
}
void CSHProactor::OnAccept(SH_HANDLE s){
}
void CSHProactor::OnMessage(SH_HANDLE s,const char *buf,int size){
}
void CSHProactor::OnClose(SH_HANDLE s){
}
bool CSHProactor::Write(SH_HANDLE s,const char *buf,int size){
    bool ret = false;
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        ret = socket->Write(buf,size);
    }
    return ret;
}
bool CSHProactor::Read(SH_HANDLE s,int size){
    bool ret = false;
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        ret = socket->Read(size);
    }
    return ret;
}
void CSHProactor::Close(SH_HANDLE s){
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        socket->Close();
    }
}
bool CSHProactor::Connect(const char *host,int port,SH_HANDLE peer){
    return m_client.Connect(host,port,(void *)peer);
}
void CSHProactor::SetPeer(SH_HANDLE s,SH_HANDLE peer){
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        socket->SetPeer(peer);
    }
}
SH_HANDLE CSHProactor::GetPeer(SH_HANDLE s){
    SH_HANDLE ret = 0;
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        ret = socket->GetPeer();
    }
    return ret;
}
CSHAioAddr CSHProactor::GetPeerAddr(SH_HANDLE s){
    CSHAioAddr ret;
    CSHAioSocket *socket = Find(s);
    if ( socket != NULL ){
        ret = socket->GetPeerAddr();
    }
    return ret;
}
