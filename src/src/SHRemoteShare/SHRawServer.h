#pragma once
#include <time.h>
#include "SHLock.h"
#include "SHAioSocket.h"

class SH_AIO_EXPORT CSHRawServer : public CSHProactor
{
public:
    CSHRawServer();
    ~CSHRawServer();
public:
	bool                Start(int port,
      	                          int maxIO,
				  int checkTimer,
				  int timeOut,
				  int threadAccount,
				  const char *proxyHost,
				  int proxyPort,
                                  int maxLockTime);
	void                Stop();
	void                SetRemote(SH_HANDLE agentId,
		                          const char *remoteHost,
								  int remotePort,
								  int maxLockTime);
	bool                IsBusy();
	void                SetBusy(bool isBusy);
	int                 GetPort();
	struct tm           GetLockTime();
	int                 GetMaxLockTime();
public:
    virtual void        OnAccept(SH_HANDLE s);
    virtual void        OnConnect(SH_HANDLE s);
    virtual void        OnMessage(SH_HANDLE s,const char *buf,int size);
    virtual void        OnClose(SH_HANDLE s);
private:
	char        m_proxyHost[256];
	int         m_proxyPort;
	SH_HANDLE   m_agentId;
	char        m_remoteHost[256];
	int         m_remotePort;
	bool        m_isBusy;
	CSHLock     m_lock;
	int         m_port;           //port listen on
	struct tm   m_lockTime;       //when has been locked
	int         m_maxLockTime;    //how long can be used to accept new connection , when status is locked
};
