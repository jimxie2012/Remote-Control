#pragma once
#include "ace/Asynch_IO.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/Asynch_Connector.h"
#include "ace/Atomic_Op.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/OS_NS_unistd.h"
#include "Asynch_RW.h"
#include "SHAio.h"
#include "ace/Addr.h"
#include "ace/Proactor.h"
#include "SHAioTask.h"
#ifdef WIN32
#define SEND_IO_PENDING       ENOBUFS //ERROR_IO_PENDING
#endif 
#ifdef __GNUC__
#define SEND_IO_PENDING       ENOBUFS
#endif 

class CAceAioServer;

class CAceAio : public ACE_Service_Handler
{   
     friend class CAceAioClient;
     friend class CAceAioServer;
     friend class CSHAio;
public:
     CAceAio();
     ~CAceAio(void);
public:
     const void *  GetAct() { return m_act; };
     void          SetAct(const void *act){ m_act = act;};
     ACE_HANDLE    GetHandle();
     bool          Read(int size);
     bool          Write(ACE_Message_Block &message_block,size_t bytes_to_write,bool withLock=true);
     bool          Close();
     bool          IsPending();
public:
     virtual void  open(ACE_HANDLE handle,ACE_Message_Block &message_block);
     virtual void  act (const void *act ) { m_act = act ;};
public:
     virtual void  handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
     virtual void  handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);
     virtual void  handle_time_out(const ACE_Time_Value &tv,const void *act = 0);
public:
     virtual void  addresses (const ACE_INET_Addr& peer, const ACE_INET_Addr& local);
private:
     int           do_delay();
     int           WriteOnce(ACE_Message_Block *mb,int size);
     bool          Read(ACE_Message_Block *mb,int size);
private:
     Asynch_RW_Stream        m_stream;
     ACE_HANDLE              handle_;
     int                     m_readIO;
     int                     m_writeIO;
     ACE_Thread_Mutex        m_mutex;
     CSHAio                 *m_shAio;
     const void *            m_act;
     ACE_Proactor           *m_proactor;
//     CSHAioTask             *m_task;
};

class CAceAioServer : public ACE_Asynch_Acceptor<CAceAio>
{
     friend class CSHAioServer;
public:
     CAceAioServer(void);
     ~CAceAioServer(void);
public:
     bool             Start(int port,int threadAccount);
     bool             Cancel();
     void             Stop();
public:
     virtual int validate_connection(const ACE_Asynch_Accept::Result& result,const ADDRESS& remote,const ADDRESS& local);
     virtual CAceAio *make_handler (void);
private:
     CSHAioServer    *m_shAio;
     CSHAioTask       m_task;
};
class CAceAioClient : public ACE_Asynch_Connector<CAceAio>
{
     friend class CSHAioClient;
public:
     CAceAioClient();
     ~CAceAioClient();
public:
     bool Start(int threadAccount);
     bool Connect(const char *ip,int port,void *act);
     bool Cancel();
     bool Stop();
public:
     virtual int validate_connection(const ACE_Asynch_Connect::Result& result,const ADDRESS& remote,const ADDRESS& local);
     virtual CAceAio *make_handler (void);
private:
     CSHAioClient *m_shAio;
     CSHAioTask    m_task;
};
