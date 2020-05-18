#include "SHHttpClient.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

CSHHttpClient::CSHHttpClient(string url){
	m_url = url;
	m_params.clear();
	m_proxyHost = "";
	m_proxyPort = 0;
}
CSHHttpClient::CSHHttpClient(){
	m_params.clear();
}
CSHHttpClient::~CSHHttpClient(void){
}
void CSHHttpClient::SetProxy(string host,int port){
	m_proxyHost = host;
	m_proxyPort = port;
}
void  CSHHttpClient::SetUrl(string url){
	m_url = url;
	m_params.clear();
}
string CSHHttpClient::GetPath(string url){
	string host = this->GetHost(url);
	int    port = this->GetPort(url);
	string path = url;
	char tmp[1024];
	sprintf(tmp,"%s:%d",host.c_str(),port);
	if ( strstr(url.c_str(),tmp) != NULL )
	    sprintf(tmp,"http://%s:%d",host.c_str(),port);
	else
	    sprintf(tmp,"http://%s",host.c_str());
	CSHString::strReplace(url,tmp,"",path);
	if ( path == "")
		path = "/";
	return path;
}
string CSHHttpClient::GetHost(string url){ 
	string host = "";
	string tmp = "";
	if (!CSHString::GetLeftSubStr(url,"//","/",tmp))
		return "";
	if (CSHString::GetLeftSubStr(tmp,"",":",host))
		return host;
	return tmp;
}
int CSHHttpClient::GetPort(string url){	
	string host = "";
	string tmp = "";
	if (!CSHString::GetLeftSubStr(url,"//","/",tmp))
		return 0;
	host = tmp;
	string port = "80";
        if (strstr(host.c_str(),":") == NULL )
            return 80;
	if ( !CSHString::GetRightSubStr(host,":","",port))
	    return 80;
	return atoi(port.c_str());
}
string CSHHttpClient::LoadUrl(string url,string param){
	string host = this->GetHost(url);
	int    port = this->GetPort(url);
	string path = this->GetPath(url);
	if ( host == "" ){
		printf("host name error\n");
		return "";
	}
	if ( port == 0 ){
		printf("port is error\n");
		return "";
	}
	char pHttpGet[1024] = "GET %s%s HTTP/1.1\r\nHost: %s:%d\r\nConnection:close\r\n\r\n"; 
    char strHttpGet[1024] = {0};
	sprintf(strHttpGet, pHttpGet, path.c_str(), param.c_str(),  host.c_str(), port);
	return this->WriteAndRead(host,port,strHttpGet);
}
string CSHHttpClient::WriteAndRead(string host,int port,string data){
	CSHAddr sa;
	if (( m_proxyHost != "" ) && ( m_proxyPort > 0 ))
		sa = CSHAddr::GetAddr(m_proxyHost.c_str(),m_proxyPort);
	else
		sa = CSHAddr::GetAddr(host.c_str(),port);

	CSHSocket sock;
	if (!sock.Create()){
		printf("create http client socket error \n");
		return "";
	}
	if ( !sock.Connect(sa) ){
		printf("can not connect to %s:%d\n",sa.GetIpString().c_str(),sa.GetPort());
		return "";
	}
	if ( strlen(data.c_str()) > 1024){
		printf("Too much data > 1024 error \n");
		return "";
	}
	CSHData netData(1024);
	char buf[1024]={'\0'};
	strcpy(buf,data.c_str());
	netData.SetData(buf,strlen(buf));
	if ( !sock.Write(netData)){
		printf("Write to restful error\n");
	    return "";
	}
	char allBuf[4096] = {'\0'};
	CSHData netDataRet(1024);
	int  allSize = 0;
	while ( sock.Read(netDataRet) ){
		int current = netDataRet.GetDataSize();
		if ( current <= 0 )
			break;
		else{
			if ( (allSize+current) >= sizeof(allBuf))
				break;
			memcpy(&allBuf[allSize],netDataRet.GetData(),current);
		    allSize = allSize + current;
		}
		netDataRet.Zero();
	}
	if ( strstr(allBuf,"OK") == NULL ){
		printf("Error Response Message\n%s\n",allBuf);
		return "";
	}
	char *pos = strstr(allBuf,"\r\n\r\n");
	if ( pos == NULL ){
		printf("Error Response Format\n%s\n",allBuf);
		return "";
	}
	return pos+strlen("\r\n\r\n");
}
string CSHHttpClient::SaveUrl(string url,string param){
	string host = this->GetHost(url);
	int    port = this->GetPort(url);
	string path = this->GetPath(url);
	if ( host == "" ){
		printf("host name error\n");
		return "";
	}
	if ( port == 0 ){
		printf("port is error\n");
		return "";
	}
	char *pHttpPost = "POST %s HTTP/1.1\r\n"
		              "Host: %s:%d\r\n"
					  "Content-Type: application/x-www-form-urlencoded\r\n"  
					  "Connection:close\r\n"
					  "Content-Length: %d\r\n\r\n"
					  "%s";
	char strHttpPost[1024] = {0};
	sprintf(strHttpPost, pHttpPost, url.c_str(), host.c_str(), port, strlen(param.c_str()), param.c_str());
	return this->WriteAndRead(host,port,strHttpPost);
}
void CSHHttpClient::AddParam(string key,string value){
	m_params.insert(make_pair(key,value));
}
string CSHHttpClient::Save(){
	string ret ="";
	map<string,string>::iterator it = m_params.begin();
    for(it=m_params.begin();it!=m_params.end();++it){
		char buf[1024];
		sprintf(buf,"%s=%s",it->first.c_str(),it->second.c_str());
		if ( strlen(ret.c_str()) > 0 )
			ret = ret + "&";
		ret = ret + buf;
	}
	return this->SaveUrl(m_url,ret);
}
string CSHHttpClient::Load(){
	string ret ="";
	map<string,string>::iterator it = m_params.begin();
    for(it=m_params.begin();it!=m_params.end();++it){
		char buf[1024];
		sprintf(buf,"/%s/%s",it->first.c_str(),it->second.c_str());
		ret = ret + buf;
	}
	while ( strstr(ret.c_str()," ") != NULL )
	    CSHString::strReplace(ret," ","%20",ret);
	return this->LoadUrl(m_url,ret);
}
string CSHHttpClient::GetHttpGetRequest(string url){
    string params = "";
    map<string,string>::iterator it = m_params.begin();
    for(it=m_params.begin();it!=m_params.end();++it){
		char buf[1024]={0};
	    sprintf(buf,"%s=%s",it->first.c_str(),it->second.c_str());
	    if ( strlen(params.c_str()) > 0 )
	        params = params + "&";
	    params = params + buf;
    }
    while ( strstr(params.c_str()," ") != NULL )
	     CSHString::strReplace(params," ","%20",params);
	
    string host = this->GetHost(url);
    int    port = this->GetPort(url);
    string path = this->GetPath(url);
    if ( host == "" ){
	    printf("host name error\n");
	    return "";
    }
    if ( port == 0 ){
	    printf("port is error\n");
	    return "";
    }
    char pHttpGet[8192] = "GET %s%s HTTP/1.1\r\nHost: %s:%d\r\nConnection:keep-alive\r\n\r\n"; 
    char strHttpGet[8192] = {0};
    sprintf(strHttpGet, pHttpGet, path.c_str(), params.c_str(),  host.c_str(), port);
    return strHttpGet;
}
string CSHHttpClient::GetHttpPostRequest(string url){
    string params = "";
    map<string,string>::iterator it = m_params.begin();
    for(it=m_params.begin();it!=m_params.end();++it){
	     char buf[1024];
	     sprintf(buf,"%s=%s",it->first.c_str(),it->second.c_str());
	     if ( strlen(params.c_str()) > 0 )
	        params = params + "&";
	     params = params + buf;
    }
    while ( strstr(params.c_str()," ") != NULL )
	     CSHString::strReplace(params," ","%20",params);
	
    string host = this->GetHost(url);
    int    port = this->GetPort(url);
    string path = this->GetPath(url);
    if ( host == "" ){
	    printf("host name error\n");
	    return "";
    }
    if ( port == 0 ){
	    printf("port is error\n");
	    return "";
    }
    char *pHttpPost = "POST %s HTTP/1.1\r\n"
	                  "Host: %s:%d\r\n"
	   	              "Content-Type: application/x-www-form-urlencoded\r\n"  
		              "Connection:keep-alive\r\n"
		              "Content-Length: %d\r\n\r\n"
		              "%s";
    char strHttpPost[8192] = {0};
    sprintf(strHttpPost, pHttpPost, url.c_str(), host.c_str(), port, strlen(params.c_str()), params.c_str());
    return strHttpPost;
}
