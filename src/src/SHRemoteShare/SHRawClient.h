#pragma once
#include "SHUdpPulse.h"
#include "SHAioSocket.h"
#include "SHLock.h"
#include "SHSessionData.h"
#include "SHCommon.h"

class CSHRawClient : public CSHProactor
{
public:
    CSHRawClient();
    ~CSHRawClient();
public:
    bool                Start(int maxIO,
			      int checkTimer,
			      int timeOut,
			      int threadAccount,
			      SH_AGENT_ID agentId,
                              const char *passWord,
                              const char *proxyHost,
	 		      int pulsePort1,
                              int pulsePort2,
                              int pulsePort3,
                              int sleepTime);
    void                Stop();
    
	void                AckUdp(CSHDataHead &info);
public:
    virtual void        OnAccept(SH_HANDLE s);
    virtual void        OnConnect(SH_HANDLE s);
    virtual void        OnMessage(SH_HANDLE s,const char *buf,int size);
    virtual void        OnClose(SH_HANDLE s);
private:
    CSHDataHead *       FindSession(SH_HANDLE s); 
    void                RemoveSession(SH_HANDLE s);
    CSHDataHead *       UpdateSession(SH_HANDLE proxyHandle,SH_HANDLE localHandle);
private:
	map<SH_HANDLE,CSHDataHead *> *m_map;
    CSHLock                       m_lock; 
	CSHUdpPulse                   m_pulse;
};
