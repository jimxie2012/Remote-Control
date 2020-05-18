#include "SHUdpSocket.h"
#include "SHString.h"
#include "SHCommon.h"
#include <string.h>
#include <errno.h>
/*
#ifdef _LINUX
  #include <linux/if_packet.h>
  #include <linux/if_ether.h>
  #include <linux/if.h>
  #include <sys/sendfile.h>
  #include <netinet/tcp.h>
#endif
#if defined(_LINUX) || defined (_DARWIN)
  #include <sys/time.h>
  #include <sys/uio.h>
  #include <unistd.h>	
  #include <fcntl.h>
#endif
#ifdef WIN32
  #include <io.h>
  #include <winsock2.h>
#endif
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include "SHDnsHead.h"
#include "SHEncrypt.h"
#ifdef WIN32
typedef int socklen_t;
#include <winsock2.h>
#define SHUT_RDWR1              2
#define CLOSE(a)               closesocket(a)
#define SETSOCKOPT(a,b,c,d,e)  setsockopt(a,b,c,(char *)d, (int)e)
#define RECVFROM(a,b,c,d,e,f)  recvfrom(a, (char *)b, c, d, (sockaddr *)e, (int *)f)
#endif
#ifdef _LINUX
#define SHUT_RDWR1              2
#define CLOSE(a)               close(a)
#define SETSOCKOPT(a,b,c,d,e)  setsockopt(a,b,c,(char *)d, (int)e)
#define RECVFROM(a,b,c,d,e,f)  recvfrom(a, (char *)b, c, d, (sockaddr *)e, f)
#endif
bool SH_GetWithLog();
bool CSHUdpSocket::SetTime(int timeOut){
#ifdef WIN32
	int useconds = timeOut*1000;
#endif
#ifdef _LINUX		
	int useconds = timeOut;
#endif
	bool ret = true;
	struct timeval   t1;  
	t1.tv_sec = useconds;
	t1.tv_usec = 0;
	int on = 1;
        m_time = timeOut;
        //m_lock_socket.Lock();
        try{
	    if(SETSOCKOPT(m_socket,SOL_SOCKET,SO_REUSEADDR/*SO_SNDTIMEO*/,(char *)&on,sizeof(on))!=0) 
		ret = false;
	    if(SETSOCKOPT(m_socket,SOL_SOCKET,SO_RCVTIMEO/*SO_SNDTIMEO*/,(char *)&t1,sizeof(t1))!=0) 
		ret = false;
	    if(SETSOCKOPT(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&t1,sizeof(t1))!=0) 
		ret = false;
        }
        catch(...){
            ret = false;
        }
        //m_lock_socket.UnLock();
	return ret;
}
bool CSHUdpSocket::ResetSocket(){
	bool ret = false;
	if ( SH_GetWithLog () ){
		CSHCommon::AddLog(SH_LOG_DEBUG,"enter reset socket\n");
	}		
	try{
		CSHUdpSocket::Close();
	}
	catch(...){     	
		if ( SH_GetWithLog () ){
			CSHCommon::AddLog(SH_LOG_DEBUG,"reset socket socket close exception\n");
		}
	}
	try{
		if ( !CSHUdpSocket::Create() ){
			if ( SH_GetWithLog () ){
				CSHCommon::AddLog(SH_LOG_DEBUG,"reset socket create failed\n");
			}
	    }
		else{
			ret = true;
			if ( SH_GetWithLog () ){
				CSHCommon::AddLog(SH_LOG_DEBUG,"reset socket create ok\n");
			}
		}
	}
    catch(...){			
		if ( SH_GetWithLog () ){
			CSHCommon::AddLog(SH_LOG_DEBUG,"reset socket create exception\n");
		}
	}
	if ( SH_GetWithLog () ){
		CSHCommon::AddLog(SH_LOG_DEBUG,"leave reset socket\n");
	}	
	return ret;
}
bool CSHUdpSocket::Create(int time){
        bool ret = true;
        int time1 = time;
        if ( time >= 30 )
            time1 = 15;
        //m_lock_socket.Lock();
        try{
	    if((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
		ret = false;  
            }
        }
        catch(...){
           ret = false;
        }
        //m_lock_socket.UnLock();
        if (ret){
            ret = SetTime( time1 );
        }
	return ret;
}
bool CSHUdpSocket::Connect(CSHAddr &addr){
     struct sockaddr_in sa = addr.GetAddr();
     if( connect(m_socket, (struct sockaddr *)(&sa), sizeof(struct sockaddr)) < 0 ) {
        return false ;
     }
     return true;
}
bool CSHUdpSocket::Bind(int port,const char *ip){
	m_localAddr.SetAddr(ip,port);
	struct sockaddr_in sa = m_localAddr.GetAddr();
	if(::bind(m_socket, (struct sockaddr *)(&sa), sizeof(struct sockaddr)) < 0 )
	     return false;
	return true;
}
bool CSHUdpSocket::ReadPulse(CSHDataPulse& data){
	bool ret = false;
	int nBytesReceived = Read((char *)data.GetData(),data.GetSize());
    if  ( nBytesReceived > 0 ){
            CSHDnsHead dns1;
            //printf("%s\n",data.GetString().c_str());
            string tt = dns1.DeRequest((char *)data.GetData(),data.GetSize());
            if ( !data.SetString(tt)){
                   printf("decode error\n");
                   return false;
            }
            //printf("cmd = %d id = %d, from %s:%d\n",data.GetCmd(),data.GetId(),m_remoteAddr.GetIpString().c_str(),m_remoteAddr.GetPort());
            if ( data.GetCmd() == SH_DETECT ){
                   string tmp = data.GetString();
                   CSHDnsHead dns;
                   dns.EnRequest((char *)tmp.c_str());
                   m_lock.Lock();
                   Write(m_remoteAddr,(char *)dns.m_buf,dns.m_size);
                   m_lock.UnLock();
                   return false;
            }
            if ( data.GetCmd() == SH_NOOP ){
                int maxTime = m_sleepTime;
                if ( m_sleepTime < 60 )
                    maxTime = 60;
                data.SetAgentAddr(m_remoteAddr.GetIpString().c_str(),m_remoteAddr.GetPort());
                /*
                if ( !m_list.IsActive(data.GetId(),maxTime) ){
                    data.SetCmd(SH_SET_PORT);
                    string tmp = data.GetString();
                    CSHDnsHead dns;
                    dns.EnRequest((char *)tmp.c_str());
                    m_lock.Lock();
                    Write(m_remoteAddr,(char *)dns.m_buf,dns.m_size);
                    m_lock.UnLock();
                }
		*/
                ret = m_list.Add(data.GetId(),*( (CSHDataPulse::ST_DATA *)data.GetData() ) );
                CSHDataHead res(SH_NOOP);
                string tmp = res.GetString();
                CSHDnsHead dns;
                dns.EnRequest((char *)tmp.c_str());
                m_lock.Lock();
                Write(m_remoteAddr,(char *)dns.m_buf,dns.m_size);
                m_lock.UnLock();
                  
                //m_lock.Lock();
                //ret = Write(m_remoteAddr,(char *)data.GetData(),data.GetSize());
                //m_lock.UnLock();
            }
        }
        return ret;
}
bool CSHUdpSocket::Write(CSHAddr addr,char *buf,int size){
	bool ret = false;
        //m_lock_socket.Lock();
        try{
	    struct sockaddr_in sa = addr.GetAddr();
	    struct sockaddr &s = *((struct sockaddr *)(&sa));         
	    if  ( ::sendto(m_socket, (const char *)buf,size, 0, &s, sizeof(struct sockaddr)) > 0 ){
	 	ret = true;
	    }
        }
        catch(...){
        }
        //m_lock_socket.UnLock();
	return ret;
}
int CSHUdpSocket::Read(char *buf,int maxSize){
    struct sockaddr addr;
    int nBytesReceived = -1;
    //m_lock_socket.Lock();
    try{
        memset(&addr,0,sizeof(addr));
        int nFromSize = sizeof(struct sockaddr);
        nBytesReceived = (int)RECVFROM(m_socket,buf,maxSize,0,&addr,(socklen_t*)&nFromSize);
        if  ( nBytesReceived > 0 ){
            m_remoteAddr.SetAddr(*((struct sockaddr_in *)&addr));
            return nBytesReceived;
        }
    }
    catch(...){
    }
    //m_lock_socket.UnLock();
    return nBytesReceived;
}
void CSHUdpSocket::Close(){
    //m_lock_socket.Lock();
	try{
		CLOSE(m_socket);
    }
    catch(...){
    }
    try{
	 //shutdown(m_socket,SHUT_RDWR1);
    }
    catch(...){
    }
    //m_lock_socket.UnLock();
}
CSHAddr& CSHUdpSocket::GetPeer(){
     return m_remoteAddr;
}
CSHUdpSocket::CSHUdpSocket() {
	 m_socket = 0;
	 m_time = SOCKET_TIME_OUT;
}
CSHUdpSocket::~CSHUdpSocket(){
	 Close();
}
int CSHUdpSocket::GetTime(){
	 return m_time;
}
bool CSHUdpSocket::SendPulse(){
	if ( SH_GetWithLog () ){
	    CSHCommon::AddLog(SH_LOG_DEBUG,"enter SendPulse\n");
    }
    CSHDataPulse data;
    data.SetCmd(SH_NOOP);
    //string hostName = CSHAddr::GetLocalHostName();
    string localIp = CSHAddr::GetLocalIpAddress();
    vector<string> vec;
    CSHString::ConvertString2Vector(vec,localIp);
    for ( int i = 0 ; i < vec.size() ; i ++ ){
	string ip = vec.at(i);
	if ( vec.at(i) != "127.0.0.1"){
	   localIp = ip;
	   break;
	}
    }
    if ( localIp == "" )
	    localIp = "127.0.0.1";
    string cfgFile = CSHCommon::GetExePath()+"shremote.cfg";
    char adminUser[63] = { 0 };
    if (!CSHCommon::ReadConfig(cfgFile.c_str(),"client","agentAdmin",adminUser))
		strcpy(adminUser,"");

    if (!CSHCommon::ReadConfig(cfgFile.c_str(),"client","agentAcl",m_acl))
		strcpy(m_acl,"RSVN");

    char temp[1024] = { 0 };
    if (CSHCommon::ReadConfig(cfgFile.c_str(),"client","agentId",temp)){
		m_id = CSHCommon::Atoul(CSHEncrypt::Decrypt(temp).c_str());
    }
	CSHAddr remoteAddr(m_proxyHost.c_str(),m_proxyPort);
    remoteAddr.SetAddr(m_proxyHost.c_str(),m_proxyPort);
    data.SetProxyAddr(m_proxyHost.c_str(),m_proxyPort);
    data.SetAgentAddr(m_localAddr.GetIpString().c_str(),m_localAddr.GetPort());
	
    data.SetId(m_id);
    data.SetAcl(m_acl);
    data.SetPrivate(localIp.c_str(),adminUser);
    string tmp = data.GetString();
    CSHDnsHead dns;
    dns.EnRequest((char *)tmp.c_str());
	if ( SH_GetWithLog () ){
		CSHCommon::AddLog(SH_LOG_DEBUG,"begin send %s:%d[%s]\n",remoteAddr.GetIpString().c_str(),remoteAddr.GetPort(),tmp.c_str());
	}
	
  	static tm dt1 = CSHCommon::GetCurrentDateTime();
	tm dt2 = CSHCommon::GetCurrentDateTime();
	unsigned int seconds = CSHCommon::GetSeconds(dt2,dt1);
	if (seconds > 7200 ){
	    dt1 = dt2;
		for ( int i = 0 ; i < 30 ; i ++ ){
			if ( CSHUdpSocket::ResetSocket() ){
				break;
			}else{
				CSHCommon::Sleep(1);
			}
		}
	}
	else{
		bool isAlive = IsServerAlive(remoteAddr);
		if ( SH_GetWithLog () ){
			CSHCommon::AddLog(SH_LOG_DEBUG,"%s:%d server is alive %d\n",remoteAddr.GetIpString().c_str(),remoteAddr.GetPort(),isAlive);
		}
		if ( !isAlive) {
			if ( SH_GetWithLog () ){
				CSHCommon::AddLog(SH_LOG_DEBUG,"%s:%d server is not alive re-connecting\n",remoteAddr.GetIpString().c_str(),remoteAddr.GetPort());
			}		
		    for ( int i = 0 ; i < 30 ; i ++ ){
				if ( CSHUdpSocket::ResetSocket() ){
					break;
				}else{
					CSHCommon::Sleep(1);
				}
			}
			//this->Connect(remoteAddr);	
		}
	}
	if ( SH_GetWithLog () ){
	    CSHCommon::AddLog(SH_LOG_DEBUG,"enter Write Pulse\n");
    }
    bool ret = Write(remoteAddr,dns.m_buf,dns.m_size);
	if ( !ret) {
		for ( int i = 0 ; i < 30 ; i ++ ){
			if ( CSHUdpSocket::ResetSocket() ){
				ret = Write(remoteAddr,dns.m_buf,dns.m_size);
				if ( ret ){
					break;
				}
			}else{
				CSHCommon::Sleep(1);
			}
		}
	}
	if ( SH_GetWithLog () ){
	    CSHCommon::AddLog(SH_LOG_DEBUG,"leave Write %d\n",ret);
    }
 	return ret;
}
bool CSHUdpSocket::ReadConnect(CSHDataHead &info){
        bool ret = false;
        int nBytesReceived = Read((char *)info.GetData(),info.GetSize());
        if  ( nBytesReceived > 0 ){		
	    CSHDnsHead dns1;
	    string tt = dns1.DeRequest((char *)info.GetData(),info.GetSize());
	    if ( !info.SetString(tt))
	        return false;
            if ( info.GetCmd() == SH_NOOP ){
            }
	    ret = true;
        }
        return ret;
}
bool CSHUdpSocket::SendConnect(SH_AGENT_ID id,CSHDataHead &info){
      CSHAddr addr;
      if ( !m_list.Find(id,addr))
           return false;    
      if ( !m_list.IsActive(id,m_sleepTime)){
           return false;
      }
      string tmp = info.GetString();
      CSHDnsHead dns;
      dns.EnRequest((char *)tmp.c_str());
      m_lock.Lock();
	  bool ret = Write(addr,(char *)dns.m_buf,dns.m_size);
      m_lock.UnLock();
      return ret;
}
bool CSHUdpSocket::ReadConnect(CSHDataConnect &info){
     bool ret = false;
     int nBytesReceived = Read((char *)info.GetData(),info.GetSize());
     if  ( nBytesReceived > 0 ){
           if ( info.GetCmd() == CMD_CONNECT ){
           }
	   ret = true;
     }
     return ret;
}
bool CSHUdpSocket::SendConnect(CSHDataConnect &info){
     return this->SendConnect(this->m_remoteAddr,info);
}
bool CSHUdpSocket::SendConnect(CSHAddr &addr,CSHDataConnect &info){
     return this->Write(addr,(char *)info.GetData(),info.GetSize());
}
void CSHUdpSocket::SetProxy(string proxyHost,int proxyPort){	
     m_proxyHost = proxyHost;
     m_proxyPort = proxyPort;
}
void CSHUdpSocket::SetAgent(SH_AGENT_ID id,const char *acl){
     m_id = id;
     strcpy(m_acl,acl);
}
int CSHUdpSocket::GetSleepTime(){
	 int ret = m_sleepTime;
	 if ( ret <= 0 )
		 ret = 5;
     return m_sleepTime;
}
void CSHUdpSocket::SetSleepTime(int time){
     m_sleepTime = time;
}
SH_AGENT_ID CSHUdpSocket::GetId(){
     return m_id;
}
bool CSHUdpSocket::IsServerAlive(CSHAddr &addr){
	bool ret = false;
	CSHUdpSocket s;
	CSHDataPulse data;
	data.SetId(GetId());
    data.SetCmd(SH_DETECT);
    data.SetAcl(m_acl);
    data.SetPrivate("127.0.0.1","test");
	if (s.Create(10) ){
		CSHDnsHead dns;
		string tmp = data.GetString();
        dns.EnRequest((char *)tmp.c_str());
		if (s.Write(addr,dns.m_buf,dns.m_size)){
			data.SetCmd(SH_NOOP);
			struct sockaddr addr1;
			memset(&addr1,0,sizeof(addr1));
			int nFromSize = sizeof(struct sockaddr);			
			int nBytesReceived = s.Read(data.GetData(),data.GetSize());
			//CSHCommon::AddLog(SH_LOG_DEBUG,data.GetData());
			if  ( nBytesReceived > 0 ){
				ret = true;
			}
		}
		s.Close();
	}
	return ret;
}
