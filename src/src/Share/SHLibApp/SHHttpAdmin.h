#pragma once
#include "SHString.h"
#include "Common.h"
#include "SHDataApp.h"

class SH_EXPORT CSHHttpAdmin
{
public:
	CSHHttpAdmin(void);
	~CSHHttpAdmin(void);
public:
	//return error:
	//0: wrong username or password
	//>0: new alloced agentid,or local agentid
	unsigned int          CheckUser(string username,string pass);
	//0:admin user is not exsit
	//1:check ok
	unsigned int          CheckAdmin(string username);
	int                   GetVersion();
private:
	string  m_urlBase;
};

