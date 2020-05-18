#include "SHHttpAdmin.h"
#include "SHHttpClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <SHConfig.h>

CSHHttpAdmin::CSHHttpAdmin(void){	
    m_urlBase = CSHConfig::GetRestfulUrl();
}
CSHHttpAdmin::~CSHHttpAdmin(void){
}
unsigned int CSHHttpAdmin::CheckUser(string username,string pass){
	string url = m_urlBase + "checkuser";
	CSHConfig cfg("client");
	cfg.Load();
	CSHHttpClient http(url);
	if ( cfg.m_isRestProxy )
		http.SetProxy(cfg.m_restProxyHost,cfg.m_restProxyPort);
	else
		http.SetProxy("",0);
	http.AddParam("username",username);
	http.AddParam("password",pass);
	string info = http.Save();
	if ( info == "")
		return 0;
	return CSHCommon::Atoul(info.c_str());
}
unsigned int CSHHttpAdmin::CheckAdmin(string username){
	string url = m_urlBase + "checkadmin";
	CSHHttpClient http(url);
	CSHConfig cfg("client");
	cfg.Load();
	if ( cfg.m_isRestProxy )
		http.SetProxy(cfg.m_restProxyHost,cfg.m_restProxyPort);
	else
		http.SetProxy("",0);
	http.AddParam("username",username);
	string info = http.Save();
	if ( info == "")
		return 0;
	return CSHCommon::Atoul(info.c_str());
}
int CSHHttpAdmin::GetVersion(){
	string url = m_urlBase + "getversion";
	CSHHttpClient http(url);
	CSHConfig cfg("client");
	cfg.Load();
	if ( cfg.m_isRestProxy )
		http.SetProxy(cfg.m_restProxyHost,cfg.m_restProxyPort);
	else
		http.SetProxy("",0);
	string info = http.Save();
	if ( info == "")
		return 0;
	return CSHCommon::Atoul(info.c_str());
}
