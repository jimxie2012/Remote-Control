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
#include "ace/OS_NS_sys_socket.h"
#include "ace/os_include/netinet/os_tcp.h"
#include "ace/SUN_Proactor.h"
#include "SHAio.h"
#include "SHCommon.h"

CAceAio::CAceAio(){
    m_shAio = NULL;
    handle_      = ACE_INVALID_HANDLE;	
    m_mutex.acquire();
    m_readIO     = 0;
    m_writeIO    = 0;
    m_mutex.release();
}
CAceAio::~CAceAio(void){
	if ( this->handle() != ACE_INVALID_HANDLE )
		ACE_OS::closesocket (this->handle_);
}
int CAceAio::do_delay (){
    int rc = 0;
    u_long millisec = RE_SEND_DELAY;
#if defined _WIN32
    ::Sleep (millisec);
#else // _WIN32
    ::usleep(millisec*1000);
    /*
    timeval  tv;
    do{
	tv.tv_sec  = millisec/1000;
	tv.tv_usec = (millisec%1000)*1000;
	rc = ::select (0,0,0,0,&tv);
     }while (rc < 0 && ACE_OS::last_error() == EINTR);
     */
#endif // _WIN32
     return rc;	
}
ACE_HANDLE CAceAio::GetHandle(){
    return handle_;
}
int CAceAio::WriteOnce(ACE_Message_Block *mb,int size){	
    try { ///add by jim 2018-02-26
        int flag = this->m_stream.write(*mb,size);
        if (flag == 0)
           return 0;
        if ( flag == -2 )
           return -1;
        if ((EAGAIN==errno)|| (EWOULDBLOCK == errno ) || ( ENOBUFS == errno ) || (errno==SEND_IO_PENDING)){
           ACE_ERROR ((LM_ERROR,"%p errno=%d\n","ACE_Asynch_Write_Stream::write",errno));
           return 1;
        }
        ACE_ERROR ((LM_ERROR,"%p errno=%d\n","ACE_Asynch_Write_Stream::write error",errno));
    }
    catch(...){
        ACE_ERROR ((LM_ERROR,"%p\n","CAceAio::WriteOnce exception"));
    }
    return -1;
}
bool CAceAio::Write(ACE_Message_Block &message_block,size_t bytes_to_write,bool withLock){
	if ( withLock )
        m_mutex.acquire ();	
    bool ret = false;
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAio::Write %d called %d ios\n",bytes_to_write,m_writeIO));
	struct tm  begin = CSHCommon::GetCurrentDateTime();
	while ( true ){
		int flag = WriteOnce(&message_block,bytes_to_write);
		if ( flag < 0 ){	
			ret = false;
			break;
		}
		if (flag == 0 ){		
			ret = true;
			break;
		}
		if (flag > 0 ){
			struct tm now = CSHCommon::GetCurrentDateTime(); 
			unsigned int seconds =CSHCommon::GetSeconds(now,begin);
			if ( seconds > m_shAio->m_timeOut ){
				ret = false;
				ACE_ERROR ((LM_ERROR,"%p\n","Write Error breaked"));
				break;
			}
		}
		ACE_ERROR ((LM_ERROR,"%p\n","Write Warrning Retry"));
		do_delay();
	} 
    if ( !ret )
	    message_block.release();
	else
	    m_writeIO++; 
	if ( withLock )
	   m_mutex.release();
    return ret;
}
bool CAceAio::Close(){    
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAio::Close called\n"));
    m_mutex.acquire();
    bool ret = true;    
	try{
		if (this->m_stream.cancel_and_close() == -1){
			ret = false;
			ACE_ERROR ((LM_ERROR,"%p\n","handle_read_stream: Failed to cancel the read"));
		}
		if ( this->handle() != ACE_INVALID_HANDLE ){
			//ACE_OS::shutdown(this->handle(),ACE_SHUTDOWN_BOTH);
			if ( ACE_OS::closesocket(this->handle()) != -1 ){
                                ACE_OS::shutdown(this->handle(),ACE_SHUTDOWN_BOTH);
				this->handle(ACE_INVALID_HANDLE);
			}
			else{
				if ( ACE_OS::shutdown(this->handle(),ACE_SHUTDOWN_BOTH)== -1 ) {
					this->handle(ACE_INVALID_HANDLE);
				}
				else{
					ret = false;
			        ACE_ERROR ((LM_ERROR,"%p\n","closesocket and shutdown: Failed"));
				}
			}
		}
	}
	catch(...){
		ACE_ERROR ((LM_ERROR,"%p\n","CAceAio::Close exception"));
	}
    m_mutex.release();
    return ret;
}
void CAceAio::addresses (const ACE_INET_Addr& peer , const ACE_INET_Addr& local){
}
void CAceAio::open(ACE_HANDLE handle,ACE_Message_Block &message_block){
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAio::open called\n"));
    m_mutex.acquire ();
    m_readIO     = 0;
    m_writeIO    = 0;
    m_mutex.release();
    this->handle_ = handle;
    this->m_stream.cancel_and_close();
    if (this->m_stream.open (*this,handle,0,m_proactor,0) == -1){
       ACE_ERROR ((LM_ERROR,"%p\n","ACE_Asynch_Read_Stream::open"));
       this->Close();
	   m_shAio->Lock();
	   m_shAio->m_isBusy = false;
       m_shAio->UnLock();
       return;
    }
    m_shAio->OnCreate();
}
bool CAceAio::Read(ACE_Message_Block *mb,int size){
    bool ret = false;	
    int act = size;
    try{ ///add by jim 2018-02-26
        if (this->m_stream.read(*mb, mb->space(),(const void *)act) != 0 )
            ACE_ERROR ((LM_ERROR,"%p\n","CAceAio::Read EAGAIN"));
        else
            ret = true;
   }
   catch(...){
       ACE_ERROR ((LM_ERROR,"%p\n","CAceAio::Read exception"));
   }
   return ret;
}
bool CAceAio::Read(int size){		
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAio::Read %d called %d ios\n",size,m_writeIO));
    ACE_Message_Block *mb=NULL;
    if ( size < 0 ){
        ACE_ERROR ((LM_ERROR,"Read size < 0 %d\n",size));
    	return false;
    }
    if ( size == 0 )
		mb = new ACE_Message_Block(SH_MSG_SIZE);
    else
		mb = new ACE_Message_Block(size);
    if ( mb ==NULL ){
       ACE_ERROR ((LM_ERROR,"cannot new mb\n"));
       return false;
    }
    mb->reset();
    m_mutex.acquire ();
    bool ret = Read(mb,size);
    if ( ret )
       m_readIO++;
	else
	   ACE_Message_Block::release(mb);
    m_mutex.release();
    return ret;
}
void CAceAio::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result){
    m_mutex.acquire ();
    ACE_DEBUG ((LM_DEBUG,"handle_read_stream called %d ios\n",m_readIO));
    m_readIO--;
    m_mutex.release();
    if (result.success () && result.bytes_transferred () != 0){	
       ACE_Message_Block &mb = result.message_block();
       int allSize = (long)result.act();
	   int readSize = result.bytes_transferred();
	   if ( readSize < allSize ){
		   int left = allSize - readSize;
		   ACE_ERROR ((LM_ERROR,"%p\n","Read not complieted will read left bytes %d",left));
		   if ( Read(&mb,left) ){
			   return;
		   }
		   mb.release ();
		   return;
	   }
	   m_shAio->OnMessage(mb.base(),mb.length());
	   mb.release ();
	}
	else{
		ACE_DEBUG((LM_DEBUG,"Receiver completed \n"));
		result.message_block().release();
		if ( IsPending() ){
			ACE_DEBUG((LM_DEBUG,"close in hand_read\n"));
			this->Close();
		}
	}
 }
void CAceAio::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result){
    m_mutex.acquire();
    ACE_DEBUG ((LM_DEBUG,"handle_write_stream called %d ios\n",m_writeIO));
    m_writeIO--;
    m_mutex.release();
    result.message_block ().rd_ptr (result.message_block ().rd_ptr () - result.bytes_transferred ());
    if ( result.success () ){
       int unsent_data = ACE_Utils::truncate_cast<int> (result.bytes_to_write () - result.bytes_transferred ());
       if (unsent_data > 0){
          result.message_block ().rd_ptr(result.bytes_transferred ());
          if ( !this->Write(*result.message_block ().duplicate(),unsent_data,false) ){
              ACE_ERROR ((LM_ERROR,"%p\n","Write not complieteed will send left bytes %d",unsent_data));
              return;
           }
		  else{
			  ACE_ERROR ((LM_ERROR,"%p\n","ReWrite Error will send left bytes %d",unsent_data));
		  }
       }
       m_shAio->OnWrite();
    }
    result.message_block().release ();  
}
void CAceAio::handle_time_out(const ACE_Time_Value &tv,const void *act){
    ACE_DEBUG((LM_DEBUG,"CAceAio::handle_time_out called\n"));
    m_mutex.acquire ();
    ACE_DEBUG((LM_DEBUG,"CAceAio::handle_time_out called wirte(%d) read(%d)\n",m_writeIO,m_readIO));
    m_mutex.release ();
    m_shAio->OnTimeOut();
	ACE_DEBUG((LM_DEBUG,"CAceAio::handle_time_out called end\n"));
}
bool CAceAio::IsPending(){
    bool ret = false;
    m_mutex.acquire ();
    if( (m_writeIO > 0 ) || ( m_readIO > 0 ) )
       ret = true;
    m_mutex.release ();
    return ret;
}
CAceAioServer::CAceAioServer(){
    m_shAio = NULL;
}
CAceAioServer::~CAceAioServer(){
}
int CAceAioServer::validate_connection(const ACE_Asynch_Accept::Result& result,const ADDRESS& remote,const ADDRESS& local){
    ACE_DEBUG((LM_DEBUG,"CAceAioServer::validate_connection called \n"));
    return 0;
}
bool CAceAioServer::Start(int port,int threadAccount){ 
   ACE_DEBUG((LM_DEBUG,"CAceAioServer::Start called port = %d,threadAccount = %d \n",port,threadAccount));
   m_task.Init(threadAccount);
   if ( this->open(ACE_INET_Addr(port),0,1,ACE_DEFAULT_ASYNCH_BACKLOG, 1,m_task.GetProactor(),0,1,-1) < 0 ){
       ACE_ERROR ((LM_ERROR,"%p\n","CAceAioServer::Start ERROR"));
       m_task.EndLoop();
       return false;
   }
   if ( this->accept (0,0) < 0) {
       ACE_ERROR ((LM_ERROR,"%p\n","CAceAioServer::accept ERROR"));
       m_task.EndLoop();
       return false;
   }
   m_task.BeginLoop();
   return true;
}
bool CAceAioServer::Cancel(){
   ACE_DEBUG((LM_DEBUG,"CAceAioServer::Cancel called \n"));
   if(ACE_Asynch_Acceptor<CAceAio>::cancel() == -1){
       ACE_ERROR ((LM_ERROR,"%p\n","CAceAioServer::Cancel ERROR"));
       return false;
   }
   return true;
}
void CAceAioServer::Stop(){
    ACE_DEBUG((LM_DEBUG,"CAceAioServer::Stop called \n"));
    m_task.EndLoop();
}
CAceAio *CAceAioServer::make_handler(void){ 
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAioServer::make_handler called\n"));
    CSHAio *sh = m_shAio->MakeIo();
    if ( sh == NULL ){
       ACE_ERROR ((LM_ERROR,"make handler is null\n"));
       return NULL;
    }
    ((CAceAio *)(sh->GetAIo()))->m_proactor = m_task.GetProactor();
    return (CAceAio *)(sh->GetAIo());
}
CAceAioClient::CAceAioClient(){
     m_shAio = NULL;
}
CAceAioClient::~CAceAioClient(){
}
bool CAceAioClient::Start(int threadAccount){
   ACE_DEBUG((LM_DEBUG,"CAceAioClient::Start called threadAccount = %d \n",threadAccount));
   m_task.Init(threadAccount);
   if ( this->open (0,m_task.GetProactor(),0)!= 0 ){
       ACE_ERROR ((LM_ERROR,"%p\n","CAceAioClient::Start ERROR"));
       m_task.EndLoop();
       return false;
   }
   m_task.BeginLoop();
   return true;
}
int CAceAioClient::validate_connection(const ACE_Asynch_Connect::Result& result,const ADDRESS& remote,const ADDRESS& local){
    return 0;
}
bool CAceAioClient::Connect(const char *ip,int port,void *act){
    ACE_INET_Addr addr;
    if (addr.set(port, ip, 1, addr.get_type ()) == -1){
        ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"), ip));
		return false;
	}
    if(ACE_Asynch_Connector<CAceAio>::connect(addr, (const ACE_INET_Addr &)ACE_Addr::sap_any,1,act)==-1){
        ACE_ERROR((LM_ERROR,"connect error\n"));
        return false;
    }
    return true;
}
bool CAceAioClient::Cancel(){
    if(ACE_Asynch_Connector<CAceAio>::cancel()==-1)
       return false;
    return true;
}
bool CAceAioClient::Stop(){
    ACE_DEBUG((LM_DEBUG,"CAceAioServer::Stop called \n"));
    m_task.EndLoop();
	return true;
}
CAceAio *CAceAioClient::make_handler(){
    ACE_DEBUG ((LM_DEBUG,"%N:%l:CAceAioClient::make_handler called\n"));
    CSHAio *sh = m_shAio->MakeIo();
    if ( sh == NULL ){
       ACE_ERROR ((LM_ERROR,"make handler is null\n"));
       return NULL;
    }
    ((CAceAio *)(sh->GetAIo()))->m_proactor = m_task.GetProactor();
    return (CAceAio *)(sh->GetAIo());
}
