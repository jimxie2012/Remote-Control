#include "SHConfig.h"
#include "SHCommon.h"
#include "SHString.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "SHEncrypt.h"
#include <string.h>
#define   SH_DEBUG   04
#define   SH_INFO    010
#define   SH_NOTICE  020
#define   SH_WARNING  040
#define   SH_ERROR  0200
#define   SH_CRITICAL  0400

CSHConfig::CSHConfig(string role){
    m_sectionName = role;
    m_fileName    = CSHCommon::GetExePath()+"shremote.cfg";
}
CSHConfig::~CSHConfig(){
}

bool CSHConfig::GetAutoRun(){
	string fileName = CSHCommon::GetExePath()+"watchdog.ini";
	char tmp[10] = {'\0'};
	if ( CSHCommon::ReadConfig(fileName.c_str(),"SYSTEM","EXTERN",tmp))
		if ( strcmp(tmp,"1")==0 )
			return true;
		else
			return false;
	return true;
}
void CSHConfig::SetAutoRun(bool autoRun){
	string fileName = CSHCommon::GetExePath()+"watchdog.ini";
    char tmp[10];
	if(autoRun)
		strcpy(tmp,"1");
	else
		strcpy(tmp,"0");
	CSHCommon::WriteConfig(fileName.c_str(),"SYSTEM","EXTERN",tmp);
}
int CSHConfig::GetVersion(){
    CSHConfig cfg("client");
    return cfg.ReadInt("version");
}	
void CSHConfig::SetVersion(int ver){
    CSHConfig cfg("client");
	cfg.WriteInt("version",ver);
}
string CSHConfig::GetDbHost(){
    CSHConfig cfg("proxy");
    return cfg.ReadString("dbHost");
}
int CSHConfig::GetDbPort(){
    CSHConfig cfg("proxy");
    return cfg.ReadInt("dbPort");
}
string CSHConfig::GetDbPass(){
    CSHConfig cfg("proxy");
    return cfg.ReadString("dbPass");
}
string CSHConfig::GetRestfulUrl(){
    CSHConfig cfg("client");
    return cfg.ReadString("restUrl");
}
string CSHConfig::ReadString(const char *szKey){
    char data[1024] = { 0 };
    if (!CSHCommon::ReadConfig(m_fileName.c_str(),m_sectionName.c_str(), szKey,data))
        return "";
    string tmp = data;
    tmp = CSHString::Trim(tmp);
    return data;
} 
bool CSHConfig::ReadBool(const char *szKey){
    if ( ReadString(szKey) == "yes" )
        return true;
    return false;       
}
int CSHConfig::ReadInt(const char *szKey){
    return atoi(ReadString(szKey).c_str());
}
unsigned int CSHConfig::ReadUInt(const char *szKey){
    string tmp = ReadString(szKey);
    if (tmp == "")
        return 0;
    return CSHCommon::Atoul(tmp.c_str());
}
bool CSHConfig::WriteString(const char *szKey,string data){
    if (!CSHCommon::WriteConfig(m_fileName.c_str(),(char *)m_sectionName.c_str(), (char *)szKey,(char *)data.c_str()))
        return false;
    return true;
}
bool CSHConfig::WriteBool(const char *szKey,bool data){
    if ( data )
       return WriteString(szKey,"yes");
    else
       return WriteString(szKey,"no");
}
bool CSHConfig::WriteInt(const char *szKey,int data){
     char tmp[1024] = { 0 };
     sprintf(tmp,"%d",data);
     return WriteString(szKey,tmp);
}
bool CSHConfig::WriteUInt(const char *szKey,unsigned int data){
     char tmp[1024] = { 0 };
     sprintf(tmp,"%u",data);
     return WriteString(szKey,tmp);
}
bool CSHConfig::Load(){
	 string temp = ReadString("logLevel");
	 if ( temp =="DEBUG")
		 m_logLevel = SH_DEBUG | SH_INFO | SH_WARNING | SH_ERROR | SH_CRITICAL;
	 else		 
		 m_logLevel = SH_WARNING | SH_ERROR | SH_CRITICAL;

     m_withLog = ReadBool("withLog");
     m_dataPort = ReadInt("dataPort");
     //if ( m_dataPort == 0 )
     //    m_dataPort = 443;
     m_pulsePort1= ReadInt("pulsePort1");
     //if ( m_pulsePort == 0 )
     //    m_pulsePort = 53;
     m_pulsePort2 = ReadInt("pulsePort2");
     m_pulsePort3 = ReadInt("pulsePort3");
     m_maxAio= ReadInt("maxAio");
     //if ( m_maxAio == 0 )
     //    m_maxAio = 1024;
     m_checkTimer= ReadInt("checkTimer");
     //if ( m_checkTimer == 0 )
     //    m_checkTimer = 1;
     m_timeOut= ReadInt("timeOut");
     //if ( m_timeOut == 0 )
     //    m_timeOut = 30;
     m_threadAccount= ReadInt("threadAccount");
     //if ( m_threadAccount == 0 )
     //    m_threadAccount = 2;
     m_sleepTime= ReadInt("sleepTime");
     //if ( m_sleepTime == 0 )
     //    m_sleepTime = 10;

     //m_agentId= ReadUInt("agentId");
     string tmpAgentId = ReadString("agentId");
     tmpAgentId = CSHEncrypt::Decrypt(tmpAgentId);
     m_agentId = CSHCommon::Atoul(tmpAgentId.c_str());

     m_agentAcl= ReadString("agentAcl");
     m_proxyHost= ReadString("proxyHost");

     m_vServerPort= ReadInt("vServerPort");
     m_maxLockTime= ReadInt("maxLockTime");
     string tmp = ReadString("vServerList");
     vector<string> vec;
     vec.clear();
     m_vServerList.clear();
     CSHString::ConvertString2Vector(vec,tmp);
     for ( int i = 0 ; i < vec.size(); i ++ ){
          int port =atoi(vec[i].c_str());
          m_vServerList.insert(m_vServerList.end(),port);
     }
	 m_agent_admin = ReadString("agentAdmin");
     m_savePass = ReadBool("savePass");
     m_autoLogIn = ReadBool("autoLogIn");
	
     m_agent_user = ReadString("agent_user");
     m_agent_user_pass= ReadString("agent_user_pass");

	 m_isRestProxy = ReadBool("isRestProxy");
     m_restProxyHost = ReadString("restProxyHost");
     m_restProxyPort = ReadInt("restProxyPort");

	 m_agent_user_pass = CSHEncrypt::Decrypt(m_agent_user_pass);
	 return true;
}
bool CSHConfig::Save(){
	 if ( m_logLevel ==SH_DEBUG | SH_INFO | SH_WARNING | SH_ERROR | SH_CRITICAL)
		 WriteString("logLevel","DEBUG");
	 else		 
		 WriteString("logLevel","ERROR");

     WriteBool("withLog",m_withLog);
     WriteInt("dataPort",m_dataPort);
     WriteInt("pulsePort1",m_pulsePort1);
     WriteInt("pulsePort2",m_pulsePort2);
     WriteInt("pulsePort3",m_pulsePort3);
     WriteInt("maxAio",m_maxAio);
     WriteInt("checkTimer",m_checkTimer);
     WriteInt("timeOut",m_timeOut);
     WriteInt("threadAccount",m_threadAccount);
     char tmpAgentId[100];
     sprintf(tmpAgentId,"%u",m_agentId);
     //WriteUInt("agentId",m_agentId);
     WriteString("agentId",CSHEncrypt::Encrypt(tmpAgentId));

     WriteString("agentAcl",m_agentAcl);
     WriteString("proxyHost",m_proxyHost);
     WriteInt("sleepTime",m_sleepTime);

     WriteInt("vServerPort",m_vServerPort);
     WriteInt("maxLockTime",m_maxLockTime);
     
     string tmp = "";
     for ( int i = 0 ; i < m_vServerList.size(); i ++ ){
     	char port[1024]={0};
     	sprintf(port,"%d",m_vServerList.at(i));
     	if ( tmp != "" )
     	    tmp = tmp+",";
        tmp = tmp + port;
     } 
     WriteString("vServerList",tmp);     

	 WriteString("agentAdmin",m_agent_admin);
	 WriteBool("savePass",m_savePass);
     WriteBool("autoLogIn",m_autoLogIn); 
     WriteString("agent_user",m_agent_user);
	 
	 tmp = CSHEncrypt::Encrypt(m_agent_user_pass);
	 WriteString("agent_user_pass",tmp);

	 WriteBool("isRestProxy",m_isRestProxy);
     WriteString("restProxyHost",m_restProxyHost);
     WriteInt("restProxyPort",m_restProxyPort);
     return true;
}
