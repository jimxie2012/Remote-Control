#pragma once
#include "SHString.h"
#include "Common.h"
#include "SHDataApp.h"
class SH_EXPORT CSHHttpService
{
public:
	CSHHttpService(void);
	~CSHHttpService(void);
	string  Add();
private:
	string  m_urlBase;
public:
	unsigned int m_id;
	unsigned int m_agentid;
	string  m_service_name;
	string  m_remote_ip;
	int     m_remote_port;
	string  m_desc;
	int     m_service_kind;
};

