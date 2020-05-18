#pragma once
#include <SHAioLock.h>
#include <SHAioList.h>
#include <iostream>
#include <list>

#define   SH_MSG_SIZE  1024
#define   RE_SEND_DELAY 100
#define   SH_HANDLE int
#define   SH_DEBUG   04
#define   SH_INFO    010
#define   SH_NOTICE  020
#define   SH_WARNING  040
#define   SH_ERROR  0200
#define   SH_CRITICAL  0400

class CSHAioList;
class CSHAioServer;

class SH_AIO_EXPORT CSHAioAddr
{
public:
      CSHAioAddr();
     ~CSHAioAddr();
public:
      unsigned int m_ip;
      int          m_port;
};
class SH_AIO_EXPORT CSHAio
{
        friend class CSHAioServer;
        friend class CSHAioClient;
        friend class CSHAioList;
public:
	CSHAio(void);
	~CSHAio(void);
public:
        static bool         Initialise(bool withLog,unsigned int logLevel);
        static void         RunTimerLoop();
public:
        CSHAioAddr          GetPeerAddr();
        unsigned int        GetTimeOut();
        void                SetAct(const void *act);
        const void *        GetAct();
        void *              GetAIo();
        bool                IsBusy();
	bool                Read(int size);
	bool                Write(const char *buf,int size);
public:
	virtual void        OnCreate();
	virtual void        OnMessage(const char *buf,int size);
	virtual void        OnWrite();
        virtual void        OnClose();
        virtual void        OnTimeOut();
private:  
        //called only by timeout,add by jim 2018-2-15
        bool                Close();
private:
        void                Release();
private:
        void               *m_io;
        struct tm           m_activeTime;
        CSHAioLock          m_lock;
	    long                m_timerId;
		int                 m_checkTimer;
public://only for AceAio
        unsigned int        m_timeOut;
        void                Lock();
        void                UnLock();
        bool                m_isBusy;
private:
        static int          disable_signal (int sigmin, int sigmax);
};
class SH_AIO_EXPORT CSHAioServer
{
public:
	CSHAioServer();
	~CSHAioServer();
public:
	bool                Start(int port,int checkTimer,int timeOut,int threadAccount);
	void                Stop();
        virtual CSHAio *    MakeIo();
private:
	void               *m_io;
        CSHAioLock          m_lock; 
        int                 m_checkTimer;
        int                 m_timeOut;
public:
        CSHAioList          m_list;
};
class SH_AIO_EXPORT CSHAioClient
{
public:
        CSHAioClient();
       ~CSHAioClient();
public:
        bool                Start(int checkTimer,int timeOut,int threadAccount);
        bool                Connect(const char *ip,int port,void *act);
        void                Stop();
        virtual CSHAio *    MakeIo();
private:
        void               *m_io;
        CSHAioLock          m_lock;
        int                 m_checkTimer;
        int                 m_timeOut;
public:
        CSHAioList          m_list;
};
