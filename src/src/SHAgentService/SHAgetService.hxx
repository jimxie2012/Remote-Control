// HXBack.h : main header file for the HXBACK application
//

#if !defined(AFX_HXBACK_H__435F2683_ABEA_4605_BC3F_754DF63B8B21__INCLUDED_)
#define AFX_HXBACK_H__435F2683_ABEA_4605_BC3F_754DF63B8B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbol

class CSHAgentApp : public CWinApp
{
public:
	CSHAgentApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHXBackApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHXBackApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HXBACK_H__435F2683_ABEA_4605_BC3F_754DF63B8B21__INCLUDED_)
