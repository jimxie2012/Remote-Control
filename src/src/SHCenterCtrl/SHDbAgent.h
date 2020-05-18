// HXDbUser.h: interface for the CSHDbUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D84__INCLUDED_)
#define AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SHMySql.h"
#include "SHDbRes.h"
#include "SHDataApp.h"
#include "SHSessionData.h"
#include "SHDbUser.h"

class  CSHDbAgent : public CSHMySql  
{
public:
	CSHDbAgent();
	 ~CSHDbAgent();
public:
	bool              Connect();
public:
        bool              DeleteTimeOutWebSession();
        bool              SetOffLine();
	bool              SetPulse(CSHDataPulse::ST_DATA data);
        bool              Load(unsigned int agent_id);
        bool              Add();
        bool              Update(unsigned int id);
private:
	void              UpdateData(bool isRead);
public:
      unsigned int        m_id;
      unsigned int        m_agent_id;
      string              m_agent_name;
      string              m_agent_user;
      string              m_agent_desc;
      string              m_private_host;
      string              m_private_ip;
      string              m_agent_ip;
      int                 m_agent_port;
      string              m_proxy_host;
      int                 m_proxy_port;
      string              m_agent_pass;
      string              m_active_time;
      int                 m_agent_stats;
private:
       CSHDbUser          m_user;
};

#endif // !defined(AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D84__INCLUDED_)
