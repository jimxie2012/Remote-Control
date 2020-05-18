#include "SHHttpAgent.h"
#include "SHHttpClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <SHConfig.h>

CSHHttpAgent::CSHHttpAgent(){
	m_urlBase = CSHConfig::GetRestfulUrl();
}
CSHHttpAgent::~CSHHttpAgent(void){
}
bool CSHHttpAgent::LoadFromLocal(){
	string fileName = CSHCommon::GetExePath()+"agent.cnf";
	char tmp[1024] = { '\0' };
	if ( !CSHCommon::ReadConfig(fileName.c_str(),"agent","id",tmp))
		return false;
	m_id = atoi(tmp);
	if ( !CSHCommon::ReadConfig(fileName.c_str(),"agent","admin_token",tmp))
		return false;
	m_admin_token = tmp;
	if ( !CSHCommon::ReadConfig(fileName.c_str(),"agent","m_nike_name",tmp))
		return false;
	m_nike_name = tmp;
	return true;
}
bool CSHHttpAgent::SaveToLocal(){
	string fileName = CSHCommon::GetExePath()+"agent.cnf";
	char buf[10] = { '\0' };
	sprintf(buf,"%d",m_id);
	if ( !CSHCommon::WriteConfig(fileName.c_str(),"agent","id",buf) )
		return false;
	if ( !CSHCommon::WriteConfig(fileName.c_str(),"agent","admin_token",(char *)m_admin_token.c_str()))
		return false;
	if ( !CSHCommon::WriteConfig(fileName.c_str(),"agent","m_nike_name",(char *)m_nike_name.c_str()))
		return false;
	return true;
}
bool CSHHttpAgent::Load(unsigned int id ){
	string url = m_urlBase + "loadagent";
	char buf[10] = { 0 };
	sprintf(buf,"%u",id);
	CSHHttpClient http(url);
	http.AddParam("id",buf);
	string info = http.Load();
	m_admin_token= "";
	m_nike_name = "";
	m_private_host= "";
	m_private_ip= "";
	if ( info != "" ){
	   CSHString::GetStrValue(info,"admin_token",m_admin_token,"#\r\n");
	   CSHString::GetStrValue(info,"nike_name",m_nike_name,"#\r\n");
	   CSHString::GetStrValue(info,"private_host",m_private_host,"#\r\n");
	   CSHString::GetStrValue(info,"private_ip",m_private_ip,"#\r\n");
	   return true;
	}
	return false;
}
string CSHHttpAgent::Add(){
	string url = m_urlBase + "addagent";
	CSHHttpClient http(url);
	http.AddParam("admin_token",m_admin_token);
	http.AddParam("nike_name",m_nike_name);
	http.AddParam("private_host",m_private_host);
	http.AddParam("private_ip",m_private_ip);
	string info = http.Save();
	m_id = atoi(info.c_str());
	return info;
}
