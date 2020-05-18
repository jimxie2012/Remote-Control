#pragma once

#include "SHAioSocket.h"
#include "SHUdpSocket.h"

class CSHRawProxy : public CSHProactor
{
public:
    CSHRawProxy();
    ~CSHRawProxy();
public:
    bool                Start(int port,
			      int udpPort1,
                              int udpPort2,
                              int udpPort3,
		              int maxIO,
			      int checkTimer,
			      int timeOut,
                              int sleepTime,
			      int threadAccount);
	void                Stop();
public:
    virtual void        OnAccept(SH_HANDLE s);
    virtual void        OnConnect(SH_HANDLE s);
    virtual void        OnMessage(SH_HANDLE s,const char *buf,int size);
    virtual void        OnClose(SH_HANDLE s);
private:
    bool                m_isWorking;
public:
    CSHUdpSocket        m_udp1;
    CSHUdpSocket        m_udp2;
    CSHUdpSocket        m_udp3;
private:
    static SH_THREAD_RETURN  UdpThread1(void *param);
    static SH_THREAD_RETURN  UdpThread2(void *param);
    static SH_THREAD_RETURN  UdpThread3(void *param);
};
