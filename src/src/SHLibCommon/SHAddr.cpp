#include <SHAddr.h>
#include <stdio.h>
#include <string.h>
//#include "Host.h"
#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include "SHString.h"
#ifdef WIN32
    #include <winsock2.h>
#endif
#ifdef _LINUX
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <resolv.h>
#endif

unsigned int  CSHAddr::GetIp()
{
	return m_ip;
}
string CSHAddr::GetIpString()
{
  string ret ="";
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(0);
  sa.sin_addr.s_addr = m_ip;
  memset(&(sa.sin_zero),0, 8);
  ret = inet_ntoa(sa.sin_addr);
  return ret;
}
int CSHAddr::GetPort()
{
    return m_port;
}
bool CSHAddr::IsTcp()
{
	return m_isTcp;
}
struct sockaddr_in &CSHAddr::GetAddr()
{
   struct sockaddr_in adr_srvr;
   memset(&adr_srvr,0,sizeof(adr_srvr));
   adr_srvr.sin_family = AF_INET;
   adr_srvr.sin_port   = ntohs(m_port);
   adr_srvr.sin_addr.s_addr = m_ip;
   return adr_srvr;
}
bool CSHAddr::SetAddr(struct sockaddr_in addr, bool isTcp)
{
	#ifdef WIN32
		m_ip = addr.sin_addr.S_un.S_addr;	    	
	#endif
	#ifdef _LINUX
		m_ip = addr.sin_addr.s_addr;	  
	#endif
	m_port  = ntohs(addr.sin_port);
	m_isTcp = isTcp;
	return true;
}
bool CSHAddr::SetAddr(unsigned int   ip, int  port, bool isTcp)
{
	m_ip = ip;
	m_port = port;
	m_isTcp = isTcp;
	return true;
}
bool CSHAddr::SetAddr(string ip, int port, bool isTcp)
{	
	string ipDomain=ip  ;
	if ( inet_addr( ipDomain.c_str ()) == INADDR_NONE )
		ipDomain = CSHAddr::GetHostByName(ip.c_str());
	if (ipDomain == "")
		return false;
	m_ip = inet_addr(ipDomain.c_str());
	m_port = port;
	m_isTcp = isTcp;
	return true;
}
bool CSHAddr::IsInvalid()
{
	bool ret = false;
    if (( m_ip <= 0 ) && ( m_port <= 0 ))
		ret = false;
	else
		ret = true;
	return ret;
}
CSHAddr::CSHAddr(string ip, int port, bool isTcp) 
{
	m_isTcp = isTcp;
	m_port = port;
	if (ip=="")
		ip="0.0.0.0";
	m_ip = inet_addr(ip.c_str());
}
CSHAddr::~CSHAddr()
{
}
bool CSHAddr::SetAddr(CSHAddr addr)
{
	m_ip = addr.m_ip;
	m_port = addr.m_port;
	m_isTcp = addr.m_isTcp;
	return true;
}
CSHAddr CSHAddr::GetAddr(string ipDomain, int port)
{
	CSHAddr ret;
	string ip = ipDomain;
	if ( inet_addr( ipDomain.c_str ()) == INADDR_NONE )
		ip = CSHAddr::GetHostByName(ipDomain.c_str());
    ret.SetAddr(ip,port);
	return ret;
}

string CSHAddr::GetHostByName(string hostName)
{
    string ret = "";
/*
#ifdef WIN32
	struct addrinfo hints;
	struct addrinfo *res=NULL, *cur=NULL;
	int sret;
	struct sockaddr_in *addr=NULL;
	char m_ipaddr[36];
	string ipString = "";
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0; 
	hints.ai_socktype = SOCK_STREAM;
	sret = getaddrinfo(hostName.c_str(), NULL,&hints,&res);
        if (sret != 0) {
            return ret;
        }
        if (res == NULL ){
            return ret;
        }
        for (cur = res; cur != NULL; cur = cur->ai_next) {
	    addr = (struct sockaddr_in *)cur->ai_addr;
	    sprintf(m_ipaddr, "%d.%d.%d.%d",(*addr).sin_addr.S_un.S_un_b.s_b1,(*addr).sin_addr.S_un.S_un_b.s_b2,(*addr).sin_addr.S_un.S_un_b.s_b3,(*addr).sin_addr.S_un.S_un_b.s_b4);
       	    ipString = m_ipaddr;
	    break;
        }
        freeaddrinfo(res);
	ret = ipString;
#endif
#ifdef _LINUX
       if(res_init() != 0 )
	   printf("init error\n");
       struct addrinfo hints;
       struct addrinfo *res=NULL, *cur=NULL;
       int sret;
       struct sockaddr_in *addr=NULL;
       char ipbuf[36] = {0};
       string ipString = "";
       memset(&hints, 0, sizeof(struct addrinfo));
       hints.ai_family = AF_INET; 
       hints.ai_flags = AI_PASSIVE;
       hints.ai_protocol = 0;
       hints.ai_socktype = SOCK_STREAM;
       sret = getaddrinfo(hostName.c_str(), NULL,&hints,&res);
       if (sret != 0) {
	   return ret;
       }
       if ( res == NULL ) {
           return ret;
       }
       for (cur = res; cur != NULL; cur = cur->ai_next) {
	   char temp[100] = {0};
	   addr = (struct sockaddr_in *)cur->ai_addr;
	   sprintf(temp,"%s\n", inet_ntop(AF_INET,&addr->sin_addr, ipbuf, 16));
	   ipString = temp;
           break;
       }
       freeaddrinfo(res);
       ret = ipString;
#endif
*/
       struct hostent *hp = ACE_OS::gethostbyname(hostName.c_str());
       if ( hp == NULL )
           return "";
       struct sockaddr_in saddr;
       ACE_OS::memcpy (&saddr.sin_addr, hp->h_addr, hp->h_length);
       CSHAddr addr;
       addr.SetAddr(saddr);
       ret = addr.GetIpString();
       return ret;
}
string CSHAddr::GetNameByHost(string ip)
{
	CSHAddr sa(ip,0);
    struct sockaddr_in addr = sa.GetAddr();
	hostent* pHostEnt = (hostent *)gethostbyaddr((char*)&(addr.sin_addr.s_addr), 4, PF_INET);
	if(pHostEnt == NULL) 
		return "";
	string ret = pHostEnt->h_name;
	delete pHostEnt;
	return ret; 
}
string CSHAddr::GetLocalHostName(){
     char hostname[256]={'\0'};//MAXHOSTNAMELEN（256）
     ACE_OS::hostname(hostname, sizeof (hostname));
     return hostname;
}
string CSHAddr::GetLocalIpAddress(){
     ACE_INET_Addr* addr_array=NULL;
     size_t count = 0;
	 int allIpCount = 0;
     string ret = "";
	 //return "127.0.0.1";
     if (ACE::get_ip_interfaces(count, addr_array) != 0){
		 if ( addr_array != NULL )
			 delete addr_array;
         return "";
	 }

     char address[INET6_ADDRSTRLEN];//可以装下IPv6地址（46），IPv4为INET_ADDRSTRLEN（16）
	 allIpCount = count;
     ACE_INET_Addr* addr_array2 = addr_array;
     while (count--){
        addr_array2->addr_to_string(address, sizeof (address));
        string tmp = address;
        string ip = "";
        if ( strstr(tmp.c_str(),":") != NULL )
             CSHString::GetLeftSubStr(tmp,"",":",ip);
        else
             ip = tmp;
        if ( strlen(ret.c_str()) > 0 )
            ret = ret + ",";
        ret = ret + ip;
        ++addr_array2;
    }
    delete [] addr_array;//记得要delete[] addr_array;
    return ret;
}
