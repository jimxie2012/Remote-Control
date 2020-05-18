#pragma once
#ifdef WIN32
#define SH_THREAD_RETURN unsigned int
#endif
#ifdef _LINUX
#define SH_THREAD_RETURN void *
#endif
#include "SHSessionData.h"
#include "SHUdpList.h"
#include "SHAddr.h"
#include "SHBlockSocket.h"
#include "SHLock.h"

class CSHUdpSocket 
{  
public:
	CSHUdpSocket();
	~CSHUdpSocket();
public:
	bool            IsServerAlive(CSHAddr &addr);
	bool            Connect(CSHAddr &addr);
	void            SetProxy(string proxyHost,int proxyPort);
	void            SetAgent(SH_AGENT_ID id,const char *acl);
	bool            Create(int time = SOCKET_TIME_OUT);
	bool            Bind(int port,const char *ip="0.0.0.0");
	bool            ReadPulse(CSHDataPulse& data);
	bool            SendPulse();
    bool            ReadConnect(CSHDataHead &info);
    bool            SendConnect(SH_AGENT_ID id,CSHDataHead &info);
	bool            ReadConnect(CSHDataConnect &info);
	bool            SendConnect(CSHDataConnect &info);
	bool            SendConnect(CSHAddr &addr,CSHDataConnect &info);
	void            Close();
	CSHAddr&        GetPeer();
	int             GetTime();
	int             GetSleepTime();
	void            SetSleepTime(int time);
	SH_AGENT_ID     GetId();
	bool            ResetSocket();
private:
    int             Read(char *buf,int maxSize);
	bool            Write(CSHAddr addr,char *buf,int size);
	bool            SetTime(int timeOut);
private:
    CSHLock         m_lock;   
	int             m_socket;
	int             m_time;
	CSHAddr         m_remoteAddr;    
public:
        CSHAddr         m_localAddr;
public:
	CSHUdpList      m_list;
	string          m_proxyHost;
	int             m_proxyPort;
	char            m_acl[128];
	SH_AGENT_ID     m_id;
	int             m_sleepTime;
};
