#pragma once
#include "SHString.h"
#include "Common.h"
#include "SHDataApp.h"
class SH_EXPORT CSHHttpAgent 
{
public:
	CSHHttpAgent();
	~CSHHttpAgent(void);
public:
	bool            LoadFromLocal();
	bool            SaveToLocal();
	bool            Load(unsigned int id);
	string          Add();
private:
	string  m_urlBase;
public:
	unsigned int m_id;
	string  m_admin_token;
	string  m_nike_name;
	string  m_private_host;
	string  m_private_ip;
};