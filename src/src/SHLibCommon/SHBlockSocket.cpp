#include "SHBlockSocket.h"
#include "SHAddr.h"
#include "ace/INET_Addr.h"
#include <iostream>
#include <string>
#include <ace/ACE.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include "ace/SOCK_Acceptor.h"
#include <ace/SOCK_Stream.h>
#include <string>
using namespace std;
#ifdef _LINUX
   #include <netdb.h>
   #include <arpa/inet.h>
   #include <netinet/in.h>
   #include <sys/socket.h>
#endif
int CSHBlockSocket::Read(char *buf,int size,bool isAllData)
{
	ACE_Time_Value timeout( GetTime(), 0);
	if ( isAllData )
	{
		int pos = 0;
		int sizeLeft = size;
		while ( sizeLeft > 0 )
		{
			int sizeRead = ((ACE_SOCK_Stream *)m_stream)->recv_n( buf+pos, sizeLeft,&timeout );
			if ( sizeRead < 0 )
				return sizeRead;
			pos = pos + sizeRead;
			sizeLeft = sizeLeft - sizeRead;
		}
		return size;
	}
	else
		return ((ACE_SOCK_Stream *)m_stream)->recv( buf, size,&timeout );
}
bool CSHBlockSocket::Write(char *buf,int size)
{	
	ACE_Time_Value timeout(GetTime(), 0);
	int pos = 0;
	int sizeLeft = size;
	while ( sizeLeft > 0 )
	{
		int bytesSend = ((ACE_SOCK_Stream *)m_stream)->send( buf+pos, size ,&timeout );
	    if ( bytesSend <= 0 )
			return false;
		pos = pos + bytesSend;
        sizeLeft = sizeLeft -bytesSend;
	}
	return true;
}
bool CSHBlockSocket::Close()
{
	((ACE_SOCK_Stream *)m_stream)->close_writer();
	((ACE_SOCK_Stream *)m_stream)->close_reader();
	if ( ((ACE_SOCK_Stream *)m_stream)->close() == 0 )
		return true;
	return false;
}
bool CSHBlockSocket::Connect(int port,string ip,int timeOut)
{	
	ACE_INET_Addr peer_addr( port, ip.c_str());
	ACE_Time_Value timeout(timeOut, 0);
	if ( ((ACE_SOCK_Connector *)m_connector)->connect( *((ACE_SOCK_Stream *)m_stream), peer_addr,&timeout ) == 0 )
	{
		return true;
	}
	return false;
}
string CSHBlockSocket::GetHostByName(string hostName)
{
	string ret = "";
	hostent* pHostEnt = gethostbyname(hostName.c_str());
	if(pHostEnt == NULL) 
		return 	ret;
	unsigned int* pulAddr = (unsigned int*) pHostEnt->h_addr_list[0];
	//SOCKADDR_IN sockTemp;
	struct sockaddr_in sockTemp;
    sockTemp.sin_family = AF_INET;
	sockTemp.sin_port = htons(0);
	sockTemp.sin_addr.s_addr = *pulAddr; // address is already in network byte order
	CSHAddr addr ;
	addr.SetAddr(sockTemp);
	return addr.GetIpString();
}
CSHBlockSocket::CSHBlockSocket() 
{
    m_stream     = new ACE_SOCK_Stream();
	m_connector  = new ACE_SOCK_Connector();
	m_time = SOCKET_TIME_OUT;
	ACE_Time_Value timeout(m_time,0);
}
CSHBlockSocket::~CSHBlockSocket()
{
	Close();
	delete ((ACE_SOCK_Stream *)m_stream);
	delete ((ACE_SOCK_Connector *)m_connector);
}
bool CSHBlockSocket::SetTime(int time)
{
	m_time = time;
	
    return true;
}
int CSHBlockSocket::GetTime()
{
	return m_time;
}
