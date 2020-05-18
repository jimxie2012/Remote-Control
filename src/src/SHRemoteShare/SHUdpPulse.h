#pragma once
#include "SHUdpSocket.h"
//#include "SHRawClient.h"

class CSHRawClient;

class CSHUdpPulse :	public CSHUdpSocket
{
	friend class CSHRawClient;
public:
	CSHUdpPulse(void);
	~CSHUdpPulse(void);
public:
	bool                      Create(SH_AGENT_ID agentId,
                                         const char *acl,
                                         const char *proxyHost,
                                         int proxyPort1,
                                         int proxyPort2,
                                         int proxyPort3,
                                         int sleepTime);
        static SH_THREAD_RETURN   PaulseThread(void *param);
	static SH_THREAD_RETURN   ListenThread(void *param);
	void                      ClosePulse(void);
private:
        bool                      SendWithResponse(string proxy_ip,int proxy_port);
        bool                      SendAllPulse();
private:
        string                    m_proxyHost;
        int                       m_proxyPort1;
        int                       m_proxyPort2;
        int                       m_proxyPort3;
	bool                      m_isPulseing;
	CSHRawClient             *m_rawClient;
        int                       m_currentPulsePort;
};
