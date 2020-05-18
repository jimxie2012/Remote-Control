#pragma once
#include "SHAio.h"
#include "SHAioLock.h"
#include <iostream>
#include <list>
#include <map>
using namespace std;

class CSHProactor;
class SH_AIO_EXPORT CSHAioSocket : public CSHAio
{
     friend class CSHProactor;
public:
     CSHAioSocket();
     ~CSHAioSocket();
public:
     CSHAioAddr          GetPeerAddr();
     bool                Write(const char *buf,int size);
     bool                Read(int size);
     void                SetPeer(SH_HANDLE s);
     SH_HANDLE           GetPeer();
     void                Close();
     virtual void        OnCreate();
     virtual void        OnMessage(const char *buf,int size);
     virtual void        OnWrite();
     virtual void        OnClose();
     virtual void        OnTimeOut();
private:
     static int          GetNextId();
private:
     CSHProactor        *m_session;
     bool                m_isClient;
     int                 m_id;
};

class  SH_AIO_EXPORT CSHProactor
{
     friend class CSHAioSocket;
public:
     CSHProactor();
     ~CSHProactor();
public:
    bool                StartServer(int port,int maxIO,int checkTimer,int timeOut,int threadAccount);
    bool                StartClient(int maxIO,int checkTimer,int timeOut,int threadAccount);
    void                StopServer();
    void                StopClient();
public:
    bool                Write(SH_HANDLE s,const char *buf,int size);
    bool                Read(SH_HANDLE s,int size);
    void                Close(SH_HANDLE s);
    bool                Connect(const char *host,int port,SH_HANDLE peer);
    void                SetPeer(SH_HANDLE s,SH_HANDLE peer);
    SH_HANDLE           GetPeer(SH_HANDLE s);
    CSHAioAddr          GetPeerAddr(SH_HANDLE s);
public:
    virtual void        OnAccept(SH_HANDLE s);
    virtual void        OnConnect(SH_HANDLE s);
    virtual void        OnMessage(SH_HANDLE s,const char *buf,int size);
    virtual void        OnClose(SH_HANDLE s);
private:
    void                AddToMap(SH_HANDLE s,CSHAioSocket *socket);
    void                RemoveFromMap(SH_HANDLE s);
    CSHAioSocket *      Find(SH_HANDLE s);
private:
    CSHAioServer                    m_server;  
    CSHAioClient                    m_client;
    CSHAioLock                      m_lock;
	map<SH_HANDLE,CSHAioSocket *>      *m_map;
};
