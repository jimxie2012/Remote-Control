#include "SHAio.h"
#include "AceAio.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_main.h"
#include "ace/Service_Config.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/Asynch_IO_Impl.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Message_Block.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/Truncate.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_sys_socket.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include "SHCommon.h"
#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

CSHAioAddr::CSHAioAddr(){
    m_ip = 0;
    m_port = 0;
}
CSHAioAddr::~CSHAioAddr(){
}
//we must disable these sig in linux
int CSHAio::disable_signal (int sigmin, int sigmax){
#if !defined (ACE_LACKS_UNIX_SIGNALS)
    sigset_t signal_set;
    if (ACE_OS::sigemptyset (&signal_set) == - 1)
       ACE_ERROR ((LM_ERROR,ACE_TEXT ("Error: (%P|%t):%p\n"),ACE_TEXT ("sigemptyset failed")));
       for (int i = sigmin; i <= sigmax; i++)
       ACE_OS::sigaddset (&signal_set, i);
# if defined (ACE_LACKS_PTHREAD_THR_SIGSETMASK)
       if (ACE_OS::sigprocmask (SIG_BLOCK, &signal_set, 0) != 0)
# else
         if (ACE_OS::thr_sigsetmask (SIG_BLOCK, &signal_set, 0) != 0)
# endif /* ACE_LACKS_PTHREAD_THR_SIGSETMASK */
            ACE_ERROR_RETURN ((LM_ERROR,ACE_TEXT ("Error: (%P|%t): %p\n"),ACE_TEXT ("SIG_BLOCK failed")),-1);
#else
       ACE_UNUSED_ARG (sigmin);
       ACE_UNUSED_ARG (sigmax);
#endif /* ACE_LACKS_UNIX_SIGNALS */
    return 0;
}
bool CSHAio::Initialise(bool withLog,unsigned int logLevel){
    ACE::init(); 
    disable_signal (ACE_SIGRTMIN, ACE_SIGRTMAX);
    disable_signal (SIGPIPE, SIGPIPE);
    disable_signal (SIGIO, SIGIO);

    if ( !withLog ){
        ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
        u_long priority_mask =ACE_LOG_MSG->priority_mask(logLevel,ACE_Log_Msg::THREAD);
        ACE_CLR_BITS (priority_mask,ACE_Log_Msg::STDERR);
    }
    else{
        string tmpFileName = CSHCommon::GetExePath();
        char tmp[1024];
        sprintf(tmp,"%sshremote.log",tmpFileName.c_str());
        ACE_OSTREAM_TYPE *output = new ofstream(tmp);
        ACE_LOG_MSG->msg_ostream (output, 1);
        ACE_LOG_MSG->priority_mask(logLevel, ACE_Log_Msg::THREAD);
        ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);
        ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
    }

#ifdef WIN32
    ACE_WIN32_Proactor *proactor_impl = new ACE_WIN32_Proactor( 65535);
#else
    //fix a bug
    ACE_POSIX_Proactor * proactor_impl = new ACE_Poll_Proactor(65535,0,0);
    //ACE_POSIX_Proactor * proactor_impl = new ACE_LINUX_Proactor(65535,0,0);
#endif
    ACE_Proactor *proactor = new ACE_Proactor( proactor_impl,1);
    if ( ACE_Proactor::instance(proactor,1) == NULL ){
         ACE_DEBUG((LM_WARNING,"ACE_Proactor::instance is null\n"));
	     return false;
	}
    return true;
}
void CSHAio::RunTimerLoop(){
    ACE_Time_Value tv(1);
    ACE_Proactor::instance()->handle_events(tv);
}
CSHAio::CSHAio(void){	
    m_io = new CAceAio();
    ((CAceAio*)m_io)->m_shAio = this;
    Lock();
    m_isBusy = false;
    UnLock();
}
CSHAio::~CSHAio(void){
    if( m_io != NULL )
       delete ((CAceAio *)m_io);
}
void CSHAio::Lock(){
    m_lock.Lock(); 
}
void CSHAio::UnLock(){
    m_lock.UnLock();
}
bool CSHAio::Close(){
	bool ret = false;
    ///called only by timeout jim
    //Lock();   //add by jim
    ACE_DEBUG((LM_DEBUG,"CSHAio::Close\n"));
    if ( m_isBusy )
       ret = ((CAceAio *)m_io)->Close();
    else
       ACE_ERROR ((LM_ERROR,"Close error is not busy\n")); 
    //UnLock(); //add by jim
	return ret;
}
void *CSHAio::GetAIo(){
    return m_io;
}
void CSHAio::SetAct(const void *act){
    Lock();
    ACE_DEBUG((LM_DEBUG,"CSHAio::SetAct\n"));
    if ( m_isBusy )
       ((CAceAio *)m_io)->SetAct(act);   
    else
       ACE_ERROR ((LM_ERROR,"SetAct error is not busy\n"));
    UnLock();
}
const void *CSHAio::GetAct(){
    const void *ret = NULL;
    Lock();
    ACE_DEBUG((LM_DEBUG,"CSHAio::GetAct\n"));
    if (m_isBusy )
        ret = ((CAceAio *)m_io)->GetAct();
    else
        ACE_ERROR ((LM_ERROR,"GetAct error is not busy\n"));
    UnLock();
    return ret;
}
CSHAioAddr CSHAio::GetPeerAddr(){
    ACE_DEBUG((LM_DEBUG,"CSHAio::GetPeerAddr\n"));
    Lock();
    CSHAioAddr ret;
    if ( m_isBusy ){
       struct sockaddr_in from_addr;
       int addrLen = sizeof(struct sockaddr_in);
       if ( ACE_OS::getpeername(((CAceAio *)m_io)->GetHandle(),(sockaddr*)&from_addr,&addrLen )!= -1){
#ifdef WIN32
          ret.m_ip = from_addr.sin_addr.S_un.S_addr;
#else
          ret.m_ip = from_addr.sin_addr.s_addr;
#endif
          ret.m_port  = ntohs(from_addr.sin_port);
       }
    }
    else{
        ACE_ERROR ((LM_ERROR,"GetPeerAddr error is not busy"));
    }
    UnLock();
    return ret;
}
bool CSHAio::IsBusy(){
    Lock();
    bool ret = m_isBusy;
    UnLock();
    return ret;
}
unsigned int CSHAio::GetTimeOut(){
    Lock();
    int ret= m_timeOut;
    UnLock();
    return ret;
}
bool CSHAio::Read(int size){
    bool ret= false;
    Lock();
    ACE_DEBUG((LM_DEBUG,"CSHAio::Read(%d)\n",size));
    if (m_isBusy )
       ret =  ((CAceAio *)m_io)->Read(size);
    else
       ACE_ERROR ((LM_ERROR,"Read error is not busy\n"));
    UnLock();
    return ret;
}
bool CSHAio::Write(const char *buf,int size){
    bool ret = false;
    ACE_DEBUG((LM_DEBUG,"CSHAio::Write(%d)\n",size));
    ACE_Message_Block *mb = new ACE_Message_Block(size+1);
    if ( mb == NULL ){
       ACE_ERROR ((LM_ERROR,"can not new mb"));
       return false;
    }
    mb->copy(buf,size);
    Lock();
    if ( m_isBusy )
       ret = ((CAceAio *)m_io)->Write(*mb,size);
    else
       ACE_ERROR ((LM_ERROR,"Write error is not busy"));
    UnLock();
    return ret;
}
void CSHAio::OnCreate(){
    ACE_DEBUG((LM_DEBUG,"OnCreate called\n"));
    Lock();
    m_activeTime = CSHCommon::GetCurrentDateTime();
    CAceAio &aio = *((CAceAio *)m_io);
    ACE_Time_Value foo_tv(m_checkTimer);
    m_timerId = aio.m_proactor->schedule_timer( aio,
                                               (void *)"timeout",
                                                ACE_Time_Value::zero,
                                                foo_tv);
/*
    m_timerId    = ACE_Proactor::instance()->schedule_timer( aio,
		                                             (void *)"timeout",
						             ACE_Time_Value::zero,
							     foo_tv);
*/
    if(m_timerId==-1)
	ACE_ERROR((LM_ERROR, "%p/n", "check_client_timeout schedule_timer\n"));
    else 
	ACE_DEBUG ((LM_DEBUG,"check_client_timeout schedule_timer registed %d\n",1));
    UnLock();
}
void CSHAio::OnMessage(const char *buf,int size){
    ACE_DEBUG((LM_DEBUG,"CSHAio::OnMessage(%d)\n",size));
    Lock();
    if ( m_isBusy )
       m_activeTime = CSHCommon::GetCurrentDateTime();
    else
       ACE_ERROR ((LM_ERROR,"OnMessage error is not busy"));
    UnLock();
}
void CSHAio::OnWrite(){
    ACE_DEBUG((LM_DEBUG,"OnWrite called begin\n"));
    Lock();
    if ( m_isBusy )
       m_activeTime = CSHCommon::GetCurrentDateTime();
    else
       ACE_ERROR ((LM_ERROR,"OnWrite error is not busy\n"));
    UnLock();
    ACE_DEBUG((LM_DEBUG,"OnWrite called end \n"));
}
void CSHAio::OnTimeOut(){
    ACE_DEBUG ((LM_DEBUG, "OnTimeOut\n"));
    Lock();
    struct tm currentTime = CSHCommon::GetCurrentDateTime();
    unsigned int noopSeconds =  CSHCommon::GetSeconds(currentTime,m_activeTime);
    UnLock();
    if ( noopSeconds > m_timeOut ){
       Lock();
       Close();
       ACE_DEBUG((LM_DEBUG,"handle = %d TimeOut %d\n",((CAceAio *)m_io)->GetHandle(),noopSeconds));
       if (!((CAceAio *)m_io)->IsPending()){
          Release();
          UnLock();
          OnClose();
       }
       else{ 
          ACE_ERROR ((LM_ERROR,"OnTimeOut is pending check next timer\n"));
	  try{
	     CAceAio *io = (CAceAio *)m_io;
	     if ( io->handle() != ACE_INVALID_HANDLE ){
		ACE_OS::shutdown(io->handle(),ACE_SHUTDOWN_BOTH);             
		io->handle(ACE_INVALID_HANDLE);
	     }
	  }
          catch(...){
	     ACE_ERROR ((LM_ERROR,"OnTimeOut shutdown socket exception\n"));
	  }
          UnLock();
       }
    }
}
void CSHAio::Release(){
    ACE_DEBUG ((LM_DEBUG, "Release called isBusy %d\n",m_isBusy));
    if ( m_isBusy ){
        ACE_DEBUG ((LM_DEBUG, "Relase cancel timer \n"));
        //ACE_Proactor::instance()->cancel_timer(m_timerId,0);
        m_isBusy = false;
    }	
    ACE_DEBUG ((LM_DEBUG, "Release Timer %d\n",m_timerId));
    CAceAio &aio = *((CAceAio *)m_io);
    if ( m_timerId >= 0 ){
      aio.m_proactor->cancel_timer(aio);
      //ACE_Proactor::instance()->cancel_timer(m_timerId,0);
      m_timerId = -1; 
   }
    ACE_DEBUG ((LM_DEBUG, "Release called end\n"));
}
void CSHAio::OnClose(){	
}

CSHAioServer::CSHAioServer(){
    m_io = new CAceAioServer(); 
    (((CAceAioServer *)m_io))->m_shAio = this;
}
CSHAioServer::~CSHAioServer(){
    Stop();
    delete (CAceAioServer *)m_io;
}
bool CSHAioServer::Start(int port,int checkTimer,int timeOut,int threadAccount){
    m_checkTimer = checkTimer;
    m_timeOut = timeOut;
    return ((CAceAioServer *)m_io)->Start(port,threadAccount);
}
void CSHAioServer::Stop(){
    ((CAceAioServer *)m_io)->Cancel();
    while ( true ){
       m_list.CloseAll();
       int busyAccount = m_list.GetBusyAccount();
       if ( busyAccount >0  ){
          ACE_DEBUG ((LM_DEBUG, "AioServer Stop error isPending \n"));
          ACE_OS::sleep(1);
       }
       else{
          break;
       }
    }
    ((CAceAioServer *)m_io)->Stop();
    ACE_DEBUG ((LM_DEBUG, "AIO Server is stopped\n"));
}
CSHAio* CSHAioServer::MakeIo(){
    ACE_DEBUG ((LM_DEBUG, "Begin AIO object \n"));
    CSHAio *ret = m_list.Alloc(m_checkTimer,m_timeOut);
    ACE_DEBUG ((LM_DEBUG, "End AIO object \n"));
    if ( ret == NULL )
       ACE_DEBUG ((LM_DEBUG, "Can not find free AIO object \n"));
    return ret;
}
CSHAioClient::CSHAioClient(){
    m_io  = new CAceAioClient();
    ((CAceAioClient *)m_io)->m_shAio = this;
}
CSHAioClient::~CSHAioClient(){
    Stop();
    delete ((CAceAioClient *)m_io);
}
bool CSHAioClient::Start(int checkTimer,int timeOut,int threadAccount){
    m_checkTimer = checkTimer;
    m_timeOut = timeOut;
    return  ((CAceAioClient *)m_io)->Start(threadAccount);
}
bool CSHAioClient::Connect(const char *ip,int port,void *act){
    return ((CAceAioClient *)m_io)->Connect(ip,port,act);
}
void CSHAioClient::Stop(){
    ((CAceAioClient *)m_io)->Cancel();
    while ( true ){
       m_list.CloseAll();
       int account = m_list.GetBusyAccount();
       if ( account > 0 ){
          ACE_DEBUG ((LM_DEBUG, "Client Stop error isPending \n"));
          ACE_OS::sleep(1);
       }
       else{
          break;
       }
    }
    ((CAceAioClient *)m_io)->Stop();
    ACE_DEBUG ((LM_DEBUG, "AIO Client is stopped\n"));
}
CSHAio *CSHAioClient::MakeIo(){
    CSHAio *ret = m_list.Alloc(m_checkTimer,m_timeOut);
    if ( ret == NULL )
       ACE_DEBUG ((LM_DEBUG, "Can not find free AIO object \n"));
    return ret; 
}
