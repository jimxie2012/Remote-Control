#pragma once
#include <list>
#include "SHLock.h"
#include "SHUdpSocket.h"
#include "SHRawServer.h"

using namespace std;

class CSHUdpServer 
{
public:
	CSHUdpServer(void);
	~CSHUdpServer(void);
public:
	static   int             AllocPort(const char *rawHost,
		                               int rawPort,
									   SH_HANDLE  agengId,
									   const char *remoteHost,
									   int remotePort,
							           int maxLockTime,
							           const char *userName,
							           const char *password);
		                                   
	bool                     StartUdp(int port);
    bool                     AppendAio(int port,
		                       int maxIO,
			    	       int checkTimer,
			  	       int timeOut,
				       int threadAccount,
				       const char *proxyHost,
				       int proxyPort,
                                       int maxLockTime);
	void                     Stop();
private:
	CSHRawServer *           FindFreeAio();
	void                     ClearAio();
    static SH_THREAD_RETURN  UdpThread(void *param);
private:
    CSHUdpSocket             m_udp;
    bool                     m_isWorking;
	CSHLock                  m_lock;
	list<CSHRawServer *>     *m_list;
};
