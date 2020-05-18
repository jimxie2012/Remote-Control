// HXDbUser.h: interface for the CSHDbUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D84__INCLUDED_)
#define AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SHMySql.h"
#include "SHDbRes.h"
#include "SHDataApp.h"
#include "SHSessionData.h"

class  CSHDbUser : public CSHMySql  
{
public:
         CSHDbUser();
	 ~CSHDbUser();
public:
	bool              Connect();
public:
        bool              Load(unsigned int agent_id);
private:
	void              UpdateData(bool isRead);
public:
      unsigned int        m_id;
      string              m_user_name;
};

#endif // !defined(AFX_HXDBUSER_H__8CB9EAC3_FD1B_4C06_8B88_2CF9EF1F2D84__INCLUDED_)
