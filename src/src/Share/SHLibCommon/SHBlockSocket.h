#pragma once
#include "Common.h"
#define   SOCKET_TIME_OUT      60

class SH_EXPORT CSHBlockSocket 
{
public:
	CSHBlockSocket();
	~CSHBlockSocket();
public:	
	bool               Connect(int port,string ip,int timeOut);
	bool               SetTime(int time = SOCKET_TIME_OUT);
	int                GetTime();
	int                Read(char *buf,int size,bool isAllData);
	bool               Write(char *buf,int size);
	bool               Close();
	static string      GetHostByName(string hostName);
private:
	int                m_time;	
	void              *m_stream;
	void              *m_connector;
};
