#pragma once
#include "Common.h"

class SH_EXPORT CSHAddr 
{
public:
	CSHAddr(string ip = "", int port = 0, bool isTcp = true);
	~CSHAddr();
public:
	static string  GetHostByName(string hostName);
	static string  GetNameByHost(string ip = "");
	static CSHAddr GetAddr(string ipDomain,int port);
	static string  GetLocalHostName();
	static string  GetLocalIpAddress();
public:
	bool               SetAddr(CSHAddr addr);
	bool               SetAddr(struct sockaddr_in addr, bool isTcp = true);
	bool               SetAddr(unsigned int   ip = 0, int  port = 0, bool isTcp = true);
	bool               SetAddr(string ip = "", int port = 0, bool isTcp = true);
	unsigned int       GetIp();
	string             GetIpString();
	int                GetPort();
	bool               IsTcp();
	struct sockaddr_in & GetAddr();
	bool               IsInvalid();
private:
	bool               m_isTcp;
	unsigned int       m_ip;
	int                m_port;
};
