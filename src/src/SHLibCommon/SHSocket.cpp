#include <SHSocket.h>
#include <string.h>
#include <algorithm>
#ifdef _LINUX
#include <linux/if_packet.h>
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
#include <Ws2tcpip.h>
#define IPTOS_LOWDELAY  0x10  /// 
#endif
#include "Host.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include "SHFile.h"

int CSHSocket::TcpSend(const char * buf, int len)
{        
	int ret = -1;
	SetSocketError(SocketSuccess);   
	do
	{
		if ( Select(SOCKET_TIME_OUT,0) <= 0 )
			break;
		ret = SEND(m_socket, (buf ), len, 0);
		TranslateSocketError();
		if ( ret <= 0 )
			break;
	} while (GetSocketError() == SocketInterrupted);
	return ret;
}
int CSHSocket::TcpRead(char *buf, int len)
{	
	int ret = 0;
	SetSocketError(SocketSuccess);       
	do 
	{
		if ( Select(SOCKET_TIME_OUT,0) <= 0 )
			break;
		ret = RECV(m_socket, (buf + ret), len, 0);
		TranslateSocketError();
		if ( ret <= 0 )
			break;
	} while ((GetSocketError() == SocketInterrupted));
	return ret;
}
bool CSHSocket::Create(bool isTcp)
{
	m_isTcp = isTcp;
	int nType = (isTcp == true) ? SOCK_STREAM:SOCK_DGRAM;
	SetSocketError(SocketSuccess);
	if((m_socket = socket(AF_INET, nType, 0)) <= 0) 
	{
		TranslateSocketError();
		return false;  
	}
	int iKeepAlive = 1;
	SETSOCKOPT(m_socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iKeepAlive, sizeof(iKeepAlive)); 
#ifdef _LINUX
	int on = 1;	
	int ret = setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if ( ret != 0 )
		return false;
#endif
	return true;
}
bool CSHSocket::Bind(CSHAddr& addr)
{
	m_localAddr.SetAddr(addr);
	struct sockaddr_in sa = addr.GetAddr();
	SetSocketError(SocketSuccess);
	if(::bind(m_socket, (struct sockaddr *)(&sa), sizeof(struct sockaddr)) < 0 )
	{	
		TranslateSocketError();
		return false;
	}
	return true;
}
bool CSHSocket::Read(CSHData& data,int timeOut)
{
	bool ret = false;
	data.Zero();
	if ( m_isTcp )
	{		
		int size = TcpRead((char *)data.GetData(), data.GetMaxSize());
		data.SetDataSize (size );
		ret = ( size > 0) ? true: false;
	}
	else
	{	
		struct sockaddr addr;
		memset(&addr,0,sizeof(addr));
		uint32 nFromSize = sizeof(struct sockaddr);
		SetSocketError(SocketSuccess);	 
		if ( Select(timeOut,0) <= 0 )
			return false;
		int nBytesReceived = RECVFROM(m_socket, (char *)data.GetData(), data.GetMaxSize(), 0, &addr, &nFromSize);
		if  ( nBytesReceived > 0 )
		{
			ret = true;
			data.SetDataSize(nBytesReceived);
			m_remoteAddr.SetAddr(*((struct sockaddr_in *)&addr));
		}
		else
		{
			TranslateSocketError();
			ret =  false;
		}
	}
	return ret;
}
bool CSHSocket::Write(CSHData data)
{
	bool ret = false;
	if ( m_isTcp )
	{	
		const char* pch1 = (const char *)data.GetData();
		int size = TcpSend(pch1, data.GetDataSize() );
		ret = ( size > 0) ? true: false;
	}
	else
	{
		struct sockaddr_in sa = m_remoteAddr.GetAddr();
		struct sockaddr &addr = *((struct sockaddr *)(&sa));  
		SetSocketError(SocketSuccess);
		if ( Select(SOCKET_TIME_OUT,0) <= 0 )
			return false;
		if  ( ::sendto(m_socket, (const char *)data.GetData(), data.GetDataSize(), 0, &addr, sizeof(struct sockaddr)) > 0 )
			ret = true;
		else
			TranslateSocketError();
	}
	return ret;
}
void CSHSocket::Close()
{	
	try
	{
		//CLOSE(m_socket);
		shutdown(m_socket,SHUT_RDWR);
		//CLOSE(m_socket);
	}
	catch(...)
	{
	}
}
bool CSHSocket::Listen(int maxClient)
{
	SetSocketError(SocketSuccess);
	if(::listen(m_socket, maxClient) < 0 )
	{
		TranslateSocketError();
		return false;
	}
	return true;
}
bool CSHSocket::Accept(CSHSocket& sClient, CSHAddr* sClientAddr)
{	
	struct sockaddr_in addr;
	uint32 nLengthAddr = sizeof(struct sockaddr);
	SetSocketError(SocketSuccess);
#ifdef WIN32
	sClient.m_socket = ::accept(m_socket, (struct sockaddr *)&addr, (int *)&nLengthAddr);
#endif
#ifdef _LINUX
	sClient.m_socket = ::accept(m_socket, (struct sockaddr *)&addr, (uint32 *)&nLengthAddr);
#endif
	if(sClient.m_socket <= 0) 
	{
		TranslateSocketError();
		return false;
	}
	if ( sClientAddr != NULL )
		sClientAddr->SetAddr(addr);
	return true;
}
bool CSHSocket::Connect(CSHAddr &addr)
{
	SetSocketError(SocketSuccess);
	m_remoteAddr.SetAddr(addr);
	if ( m_isTcp )
	{
		struct sockaddr_in sa = addr.GetAddr();
		if( connect(m_socket, (struct sockaddr *)(&sa), sizeof(struct sockaddr)) < 0 ) 
		{		
			TranslateSocketError();     
			return false ;
		}
	}
	return true;
}
CSHAddr& CSHSocket::GetPeerAddr()
{
	return m_remoteAddr;
}
CSHAddr& CSHSocket::GetLocalAddr()
{
	struct sockaddr addr;
	uint32 nLengthAddr = sizeof(struct sockaddr);
	SetSocketError(SocketSuccess);
#ifdef WIN32
	if(getsockname(m_socket, &addr,  (int *)&nLengthAddr) >= 0)
#endif
#ifdef _LINUX	
		if(getsockname(m_socket, &addr,  (uint32 *)&nLengthAddr) >= 0)
#endif
			m_localAddr.SetAddr(*(struct sockaddr_in *)&addr);
		else
			TranslateSocketError();
	return m_localAddr;
}
void CSHSocket::SetSocket(int s)
{
	m_socket = s;
}
int  CSHSocket::GetSocket()
{
	return m_socket;
}
CSHSocket::CSHSocket() 
{
	m_socket = 0;
	m_isTcp = true;
}
CSHSocket::~CSHSocket()
{
	Close();
}
void CSHSocket::SetSocketError(CSHSocket::CSocketError error)
{
	m_socketErrno = error;
}
CSHSocket::CSocketError CSHSocket::GetSocketError(void)
{
	return m_socketErrno; 
}
void CSHSocket::TranslateSocketError(void)
{
#ifdef _LINUX
	switch (errno)
	{
	case EXIT_SUCCESS:
		SetSocketError(CSHSocket::SocketSuccess);
		break;
	case ENOTCONN:
		SetSocketError(CSHSocket::SocketNotconnected);
		break;
	case ENOTSOCK:
	case EBADF:
	case EACCES:
	case EAFNOSUPPORT:
	case EMFILE:
	case ENFILE:
	case ENOBUFS:
	case ENOMEM:
	case EPROTONOSUPPORT:
		SetSocketError(CSHSocket::SocketInvalidSocket);
		break;
	case ECONNREFUSED :
		SetSocketError(CSHSocket::SocketConnectionRefused);
		break;
	case ETIMEDOUT:
		SetSocketError(CSHSocket::SocketTimedout);
		break;
	case EINPROGRESS:
		SetSocketError(CSHSocket::SocketEinprogress);
		break;
	case EWOULDBLOCK:
		//case EAGAIN:
		//	       SetSocketError(CSHSocket::SocketEwouldblock);
		//	       break;
	case EINTR:
		SetSocketError(CSHSocket::SocketInterrupted);
		break;
	case ECONNABORTED:
		SetSocketError(CSHSocket::SocketConnectionAborted);
		break;
	case EINVAL:
	case EPROTO:
		SetSocketError(CSHSocket::SocketProtocolError);
		break;
	case EPERM:
		SetSocketError(CSHSocket::SocketFirewallError);
		break;
	case EFAULT:
		SetSocketError(CSHSocket::SocketInvalidSocketBuffer);
		break;
	case ECONNRESET:
		SetSocketError(CSHSocket::SocketConnectionReset);
		break;
	case ENOPROTOOPT: 
		SetSocketError(CSHSocket::SocketConnectionReset);
		break;
	default:
		SetSocketError(CSHSocket::SocketEunknown);
		break;	
	}
#endif
#ifdef WIN32
	int nError = WSAGetLastError();
	switch (nError)
	{
	case EXIT_SUCCESS:
		SetSocketError(CSHSocket::SocketSuccess);
		break;
	case WSAEBADF:
	case WSAENOTCONN:
		SetSocketError(CSHSocket::SocketNotconnected);
		break;
	case WSAEINTR:
		SetSocketError(CSHSocket::SocketInterrupted);
		break;
	case WSAEACCES:
	case WSAEAFNOSUPPORT:
	case WSAEINVAL:
	case WSAEMFILE:
	case WSAENOBUFS:
	case WSAEPROTONOSUPPORT:
		SetSocketError(CSHSocket::SocketInvalidSocket);
		break;
	case WSAECONNREFUSED :
		SetSocketError(CSHSocket::SocketConnectionRefused);
		break;
	case WSAETIMEDOUT:
		SetSocketError(CSHSocket::SocketTimedout);
		break;
	case WSAEINPROGRESS:
		SetSocketError(CSHSocket::SocketEinprogress);
		break;
	case WSAECONNABORTED:
		SetSocketError(CSHSocket::SocketConnectionAborted);
		break;
	case WSAEWOULDBLOCK:
		SetSocketError(CSHSocket::SocketEwouldblock);
		break;
	case WSAENOTSOCK:
		SetSocketError(CSHSocket::SocketInvalidSocket);
		break;
	case WSAECONNRESET:
		SetSocketError(CSHSocket::SocketConnectionReset);
		break;
	case WSANO_DATA:
		SetSocketError(CSHSocket::SocketInvalidAddress);
		break;
	case WSAEADDRINUSE:
		SetSocketError(CSHSocket::SocketAddressInUse);
		break;
	case WSAEFAULT:
		SetSocketError(CSHSocket::SocketInvalidPointer);
		break;
	default:
		SetSocketError(CSHSocket::SocketEunknown);
		break;	
	}
#endif
	//	printf("last errno=%d\r\n",m_socketErrno);
}
CSHSocket::CSocketStatus CSHSocket::Select(int nTimeoutSec, int nTimeoutUSec)
{
	struct timeval *pTimeout = NULL;
	struct timeval  timeout;
	int             nNumDescriptors = -1;
	int             nError = 0;
	CSocketStatus ret = STATUS_ERROR;

	fd_set               m_writeFds;		  /// write file descriptor set
	fd_set               m_readFds;		      /// read file descriptor set
	fd_set               m_errorFds;		  /// error file descriptor set

	SetSocketError(SocketSuccess);
	FD_ZERO(&m_errorFds);
	FD_ZERO(&m_readFds);
	FD_ZERO(&m_writeFds);
	FD_SET(m_socket, &m_errorFds);
	FD_SET(m_socket, &m_readFds);
	FD_SET(m_socket, &m_writeFds);

	//---------------------------------------------------------------------
	// If timeout has been specified then set value, otherwise set timeout
	// to NULL which will block until a descriptor is ready for read/write
	// or an error has occurred.
	//---------------------------------------------------------------------
	if ((nTimeoutSec > 0) || (nTimeoutUSec > 0))
	{
		timeout.tv_sec = nTimeoutSec;
		timeout.tv_usec = nTimeoutUSec;
		pTimeout = &timeout;
	}
	nNumDescriptors = SELECT(m_socket+1, &m_readFds, &m_writeFds, &m_errorFds, pTimeout);
	//  nNumDescriptors = SELECT(m_socket+1, &m_readFds, NULL, NULL, pTimeout);
	if ( nNumDescriptors < 0 )
	{
		TranslateSocketError();
		return STATUS_ERROR;
	}
	//----------------------------------------------------------------------
	// Handle timeout
	//----------------------------------------------------------------------
	if (nNumDescriptors == 0) 
	{
		SetSocketError(CSHSocket::SocketTimedout);
		return STATUS_TIME_OUT;
	}
	//----------------------------------------------------------------------
	// If a file descriptor (read/write) is set then check the
	// socket error (SO_ERROR) to see if there is a pending error.
	//----------------------------------------------------------------------
	if ( FD_ISSET(m_socket, &m_readFds) )
	{    
		int nLen  = sizeof(nError);
		if (GETSOCKOPT(m_socket, SOL_SOCKET, SO_ERROR, &nError, &nLen) == 0)
		{
			errno = nError;
			if (nError == 0)
				ret = STATUS_CAN_READ;
		}  
		if ( ret == STATUS_ERROR )
			TranslateSocketError();
	}
	if ( FD_ISSET(m_socket, &m_writeFds ) )
	{
		int  nLen = sizeof(nError);
		bool flag  = false;
		if (GETSOCKOPT(m_socket, SOL_SOCKET, SO_ERROR, &nError, &nLen) == 0)
		{
			errno = nError;
			if (nError == 0)
			{
				flag = true;
				if ( ret == STATUS_CAN_READ)
					ret = STATUS_CAN_BOTH;
				else
					ret = STATUS_CAN_WRITE;
			}
		}  
		if ( !flag )
		{
			TranslateSocketError();
		}
	}
	return ret;
}

bool CSHSocket::ReadLine(string &line)
{ 
	string ret = "";
	string buffer;  
	string::iterator pos; 
	while ((pos = find (buffer.begin(), buffer.end(), '\n')) == buffer.end ())
	{
		char buf [1025];     
		CSHData data;
		if ( !Read(data))
			return false;
		memcpy(buf,data.GetData(),data.GetDataSize());
		int n = data.GetDataSize(); 
		if (n == -1)
		{
			line = buffer; 
			buffer = "";  
			return false;  
		}
		buf [n] = 0;  
		buffer += buf;
	}    
	line = string (buffer.begin(), pos); 
	buffer = string (pos + 1, buffer.end()); 
	ret = line;
	return true;
} 
int CSHSocket::GetFreePort()
{
	static int times = 0;
	times++;
	if ( times > 300 )
	{
		times = 0;
		return -1;
	}
	int port = 0;
	int fd = -1;
	int socklen_t = 0;
	port = -1;

#ifndef AF_IPV6
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(0);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd < 0)
	{
		printf("socket() error:%s\n", strerror(errno));
		return -1;
	}

	if(bind(fd, (struct sockaddr *)&sin, sizeof(sin)) != 0)
	{
		printf("bind() error:%s\n", strerror(errno));
		CLOSE(fd);
		shutdown(fd,SHUT_RDWR);
		return -1;
	}

	uint32 len = sizeof(sin);
#ifdef WIN32
	if(getsockname(fd, (struct sockaddr *)&sin,(int *) &len) != 0)
#endif
#ifdef _LINUX	
		if(getsockname(fd, (struct sockaddr *)&sin,(uint32 *) &len) != 0)
#endif
		{
			printf("getsockname() error:%s\n", strerror(errno));
			CLOSE(fd);
			shutdown(fd,SHUT_RDWR);
			return -1;
		}
		port = sin.sin_port;
		CLOSE(fd);
		shutdown(fd,SHUT_RDWR);

#else
	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	sin.sin_family = AF_INET6;
	sin.sin_port = htons(0);
	sin6.sin_addr.s_addr = htonl(IN6ADDR_ANY);

	fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

	if(fd < 0){
		printf("socket() error:%s\n", strerror(errno));
		return -1;
	}

	if(bind(fd, (struct sockaddr *)&sin6, sizeof(sin6)) != 0)
	{
		printf("bind() error:%s\n", strerror(errno));
		close(fd);
		return -1;
	}

	len = sizeof(sin6);
	if(getsockname(fd, (struct sockaddr *)&sin6, &len) != 0)
	{
		printf("getsockname() error:%s\n", strerror(errno));
		close(fd);
		return -1;
	}

	port = sin6.sin6_port;

	if(fd != -1)
		close(fd);

#endif
	if ( port > 0 )
	{
		CSHSocket s;
		s.Create(false);
		CSHAddr addr("",port,false);
		if ( !s.Bind(addr))
		{
			CLOSE(s.GetSocket());
			s.Close();
			return GetFreePort();
		}
		else
		{
			CLOSE(s.GetSocket());
			s.Close();
		}
	}
	return port;
}
int CSHSocket::ReadCmd(){	
	int ret = -1;
	if (TcpRead((char *)(&ret),sizeof(ret)) != sizeof(ret))
		return -1;
	return ret;
}
bool CSHSocket::SendCmd(int cmd){	
	if ( TcpSend((char *)(&cmd),sizeof(cmd)) != sizeof(cmd) )
		return false;
	return true;
}
bool CSHSocket::SendFile(string fileName){
	int fileSize = CSHFile::GetFileSize(fileName);
	CSHFile io;
	if (!io.Open(fileName)){
		SendCmd(-1);
		return false;
	}	
	if ( !SendCmd(fileSize)){
		io.Close();
		return false;
	}
	if ( ReadCmd() != 0 ){	
		io.Close();
		return false;
	}
	io.Seek(false);
	CSHData data(1024);
	while(io.Read(data)){
		if ( !Write(data) ){
			io.Close();
			return false;
	    }
	    if ( ReadCmd() != 0 ){
			io.Close();
		    return false;
	    }
	}
	io.Close();
	return true;
}
bool CSHSocket::ReadFile(string fileName){
	int fileSize = ReadCmd();
	if ( fileSize >= 0 ) {
		CSHFile io;
		CSHFile::Remove(fileName);
		if (!io.Open(fileName,false)){
			SendCmd(-1);
			return false;
		}
		if (!SendCmd(0)){
			io.Close();
			return false;
		}
		CSHData data(1024);
		int allSize = 0;
		while(Read(data)){
			if (!io.Write(data)){
				SendCmd(-1);
				io.Close();
				return false;
			}
			else{
				if (!SendCmd(0)){
					io.Close();
					return false;
				}
			}
			allSize = allSize + data.GetDataSize();
			if ( allSize >= fileSize ){
				break;
			}
		}
		io.Close();
		return true;
	}
	return false;
}
