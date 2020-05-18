#include "stdafx.h"
#include "SHAgetService.hxx"
#include "SHCommon.h"
#include "utilit.h"
#include "SHDataApp.h"
#include "SHProcess.h"
extern void RunClient();

BEGIN_MESSAGE_MAP(CSHAgentApp, CWinApp)
	//{{AFX_MSG_MAP(CHXBackApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()
CSHAgentApp::CSHAgentApp(){
}
CSHAgentApp theApp;
BOOL CSHAgentApp::InitInstance(){
	SetCurrentDirectory(CSHCommon::GetExePath().c_str());
	::CreateMutex(NULL,FALSE,SH_AGENT_SEQ);
	if(GetLastError()==ERROR_ALREADY_EXISTS)	
	    return FALSE;
	RunClient();
	SH_BeginLoop();
	return FALSE;
}
