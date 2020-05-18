#include "SHHttpService.h"
#include "SHHttpClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <SHConfig.h>

CSHHttpService::CSHHttpService(void){
     m_urlBase = CSHConfig::GetRestfulUrl();
}
CSHHttpService::~CSHHttpService(void){
}
string CSHHttpService::Add(){
	string url = m_urlBase + "addservice";
	CSHHttpClient http(url);
	//http.AddParam("admin_token",m_admin_token);
	//http.AddParam("nike_name",m_nike_name);
	//http.AddParam("private_host",m_private_host);
	//http.AddParam("private_ip",m_private_ip);
	string info = http.Save();
	m_id = atoi(info.c_str());
	return info;
}
