#include <string.h>
#include "SHString.h"
#include "SHCommon.h"
#include "SHSessionData.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <winsock.h>
#endif

CSHDataHead::CSHDataHead(const char *buf){
    memcpy(&m_head,buf,sizeof(m_head));
}
CSHDataHead::CSHDataHead(int cmd){	
    memset(&m_head,0,sizeof(m_head));
    m_head.m_cmd  = SH_NOOP;
    m_head.m_var  = SH_SOFT_VAR;
    m_head.m_flag = false;
    m_head.m_cmd  = cmd;
}
CSHDataHead::~CSHDataHead(){
}
void CSHDataHead::SetData(char *data,int size){
    memcpy(&m_head,data,size);
}
int CSHDataHead::GetSize(){
    return sizeof(CSHDataHead::ST_HEAD);
}
int CSHDataHead::GetCmd(){
    return m_head.m_cmd;
}
bool CSHDataHead::GetFlag(){
    return m_head.m_flag;
}
void CSHDataHead::SetCmd(int cmd){
	m_head.m_cmd = cmd;
}
void CSHDataHead::SetFlag(bool flag){
    m_head.m_flag = flag;
}
SH_HANDLE CSHDataHead::GetSession(){
    return m_head.m_session;
}
void CSHDataHead::SetSession(SH_HANDLE id){
    m_head.m_session     = id;
}
string CSHDataHead::GetRemoteHost(){
    string ret = m_head.m_remoteHost;
    return ret;
}
int CSHDataHead::GetRemotePort(){
    return m_head.m_remotePort;
}
void CSHDataHead::SetRemote(const char *host,int port){
    if (strlen(host) < sizeof(m_head.m_remoteHost) ){
        strcpy(m_head.m_remoteHost,host);
        m_head.m_remotePort = port;
    }
}
string CSHDataHead::GetProxyHost(){
    string ret = m_head.m_proxyHost;
    return ret;
}
int CSHDataHead::GetProxyPort(){
    return m_head.m_proxyPort;
}	
int CSHDataHead::GetKey(){
	return m_head.m_key;
}
void CSHDataHead::SetKey(int key){
	m_head.m_key = key;
}
void CSHDataHead::SetProxy(const char *host,int port){
    if (strlen(host) < sizeof(m_head.m_proxyHost) ){
        strcpy(m_head.m_proxyHost,host);
        m_head.m_proxyPort = port;
    }
}
void *CSHDataHead::GetData(){
    return &m_head;
}
string CSHDataHead::GetString(){
	char ret[1024] = { 0 };
	sprintf(ret,"%d&?&%d&?&%d&?&%s&?&%d&?&%s&?&%d&?&%d&?&%u&?&%d",
			    m_head.m_var,    
	            m_head.m_cmd,
	            m_head.m_flag,   
                m_head.m_remoteHost,
                m_head.m_remotePort,
                m_head.m_proxyHost,
                m_head.m_proxyPort,
                m_head.m_isTcp,
                m_head.m_session,
				m_head.m_key);  
	string retStr=ret;
	while ( strstr(retStr.c_str(),".") != NULL )
	       CSHString::strReplace(retStr,".","?#?",retStr);
    return retStr;
}
bool CSHDataHead::SetString(string str){
	vector<string> vec;
	string tmp = str;
	while ( strstr(tmp.c_str(),"?#?") != NULL )
	       CSHString::strReplace(tmp,"?#?",".",tmp);
	CSHString::ConvertString2Vector(vec,tmp,"&?&");
	if ( vec.size() == 9 ){ 
		m_head.m_var = atoi(vec.at(0).c_str());
	    m_head.m_cmd = atoi(vec.at(1).c_str());
	    m_head.m_flag = atoi(vec.at(2).c_str());
		strcpy(m_head.m_remoteHost,vec.at(3).c_str());
	    m_head.m_remotePort = atoi(vec.at(4).c_str());
		strcpy(m_head.m_proxyHost,vec.at(5).c_str());
		m_head.m_proxyPort = atoi(vec.at(6).c_str());
		m_head.m_isTcp= atoi(vec.at(7).c_str());
		m_head.m_session= atoi(vec.at(8).c_str());
		m_head.m_key=0;
		return true;
	}
	if ( vec.size() == 10 ){ 
		m_head.m_var = atoi(vec.at(0).c_str());
	    m_head.m_cmd = atoi(vec.at(1).c_str());
	    m_head.m_flag = atoi(vec.at(2).c_str());
		strcpy(m_head.m_remoteHost,vec.at(3).c_str());
	    m_head.m_remotePort = atoi(vec.at(4).c_str());
		strcpy(m_head.m_proxyHost,vec.at(5).c_str());
		m_head.m_proxyPort = atoi(vec.at(6).c_str());
		m_head.m_isTcp= atoi(vec.at(7).c_str());
		m_head.m_session= atoi(vec.at(8).c_str());
		m_head.m_key= atoi(vec.at(9).c_str());
		return true;
	}
	return false;
}
//CSHDataPulse
CSHDataPulse::CSHDataPulse(){
    SetCmd(SH_NOOP);
    m_data.m_var = SH_SOFT_VAR;
}
CSHDataPulse::~CSHDataPulse(){
}   
char * CSHDataPulse::GetData(){
    return (char *)&m_data;
}
int CSHDataPulse::GetSize(){
   return sizeof(m_data);
}
void CSHDataPulse::SetCmd(int cmd){
   m_data.m_cmd = cmd;
}
int CSHDataPulse::GetCmd(){
   return m_data.m_cmd;
}  
SH_AGENT_ID CSHDataPulse::GetId(){
   return m_data.m_id;
}
void CSHDataPulse::SetId(SH_AGENT_ID id){
    m_data.m_id = id;
}	
const char * CSHDataPulse::GetAcl(){
	return m_data.m_acl;
}
void CSHDataPulse::SetAcl(const char *acl){
	if ( strlen(acl) < (sizeof(m_data.m_acl) -1 ))
		strcpy(m_data.m_acl,acl);
	else
		strcpy(m_data.m_acl,"RVSN");
}
void CSHDataPulse::SetData(char *data,int size){
    memcpy(&m_data,data,size);
}
const char * CSHDataPulse::GetProxyHost(){
    return m_data.m_proxyHost;
}
int CSHDataPulse::GetProxyPort(){
    return m_data.m_proxyPort;
}
void CSHDataPulse::SetProxyAddr(const char *host,int port){
    strcpy(m_data.m_proxyHost,host);
    m_data.m_proxyPort = port;
}
void CSHDataPulse::SetAgentAddr(const char *ip,int port){
    strcpy(m_data.m_agentIp,ip);
    m_data.m_agentPort = port;
}
const char * CSHDataPulse::GetAgentIp(){
    return m_data.m_agentIp;
}
int CSHDataPulse::GetAgentPort(){
    return m_data.m_agentPort;
}
void CSHDataPulse::SetPrivate(const char *privateIp,const char *admin_user){
	strcpy(m_data.m_privateIp,privateIp);
	if ( strlen(admin_user) < sizeof(m_data.m_user) - 1){
	   strcpy(m_data.m_user,admin_user);
	}
	else{
		printf("admin_user's name is too long\n");
	}
}
const char *CSHDataPulse::GetPrivateIp(){
	return m_data.m_privateIp;
}
const char *CSHDataPulse::GetAdminUser(){
	return m_data.m_user;
}
string CSHDataPulse::GetString(){
	char ret[1024] = { 0 };
     sprintf(ret,"%d&?&%d&?&%u&?&%s&?&%s&?&%d&?&%s&?&%d&?&%s&?&%s",
		         m_data.m_var,
			     m_data.m_cmd,
				 m_data.m_id,
				 m_data.m_acl,
				 m_data.m_agentIp,
				 m_data.m_agentPort,
				 m_data.m_proxyHost,
				 m_data.m_proxyPort,
				 m_data.m_privateIp,
				 m_data.m_user);
	 string retStr=ret;
	 while ( strstr(retStr.c_str(),".") != NULL )
	       CSHString::strReplace(retStr,".","?#?",retStr);
     return retStr;
}
bool CSHDataPulse::SetString(string str){
	vector<string> vec;
	string tmp = str;
	while ( strstr(tmp.c_str(),"?#?") != NULL )
	       CSHString::strReplace(tmp,"?#?",".",tmp);
	CSHString::ConvertString2Vector(vec,tmp,"&?&");
	if ( vec.size() == 10 ){ 
		m_data.m_var = atoi(vec.at(0).c_str());
	    m_data.m_cmd = atoi(vec.at(1).c_str());
	    m_data.m_id = atoi(vec.at(2).c_str());
		strcpy(m_data.m_acl,vec.at(3).c_str());
		strcpy(m_data.m_agentIp,vec.at(4).c_str());
		m_data.m_agentPort = atoi(vec.at(5).c_str());
		strcpy(m_data.m_proxyHost,vec.at(6).c_str());
		m_data.m_proxyPort = atoi(vec.at(7).c_str());
		strcpy(m_data.m_privateIp,vec.at(8).c_str());
		strcpy(m_data.m_user,vec.at(9).c_str());
		return true;
	}
	return false;
}
CSHDataConnect::CSHDataConnect(const char *buf){
    memcpy(&m_head,buf,sizeof(m_head));
}
CSHDataConnect::CSHDataConnect(void){
    m_head.m_cmd  = CMD_CONNECT;
    m_head.m_var  = SH_SOFT_VAR;
    m_head.m_flag = false;
}
CSHDataConnect::~CSHDataConnect(void){
}
int CSHDataConnect::GetSize(){
    return sizeof(CSHDataConnect::ST_HEAD);
}
int CSHDataConnect::GetCmd(){
   return m_head.m_cmd;
}
bool CSHDataConnect::GetFlag(){
	return m_head.m_flag;
}
void CSHDataConnect::SetFlag(bool flag){
	m_head.m_flag = flag;
}
SH_HANDLE CSHDataConnect::GetAgentId(){
	return m_head.m_agentId;
}
string CSHDataConnect::GetRemoteHost(){
	string ret = m_head.m_remoteHost;
	return ret;
}
int CSHDataConnect::GetRemotePort(){
	return m_head.m_remotePort;
}
int CSHDataConnect::GetMaxLockTime(){
    return m_head.m_maxLockTime;
}
string CSHDataConnect::GetUser(){
	string ret = m_head.m_user;
	return ret;
}
string CSHDataConnect::GetPass(){
	string ret = m_head.m_pass;
	return ret;
}
void CSHDataConnect::SetUser(int maxLockTime,const char  *userName,const char *password){
	if ( strlen(userName) < sizeof(m_head.m_user) )
		strcpy(m_head.m_user,userName);
	if ( strlen(password) < sizeof(m_head.m_pass) )
		strcpy(m_head.m_pass,password);
	m_head.m_maxLockTime = maxLockTime;
}
void CSHDataConnect::SetRemote(SH_HANDLE agentId,const char *host,int port){
	if ( strlen(host) < sizeof(m_head.m_remoteHost) )
		strcpy(m_head.m_remoteHost,host);
	m_head.m_remotePort = port;
	m_head.m_agentId = agentId;
}
void *CSHDataConnect::GetData(){
     return &m_head;
}
void CSHDataConnect::SetResponsePort(int port){
     m_head.m_responsePort = port;
}
int CSHDataConnect::GetResponsePort(){
	return m_head.m_responsePort;
}
void CSHDataConnect::SetData(char *data,int size){
    memcpy(&m_head,data,size);
}
/*
string CSHDataConnect::GetString(){
	char ret[1024] = { 0 };
	 string retStr=ret;
     return retStr;
}
bool CSHDataConnect::SetString(string str){
	return false;
}
*/
