#pragma once
#include <string>
#include <SHSocket.h>
#include <SHString.h>
#include <Common.h>
class SH_EXPORT CSHHttpClient
{
public:
	CSHHttpClient(string url);
	CSHHttpClient();
	~CSHHttpClient(void);
public:
	string Save();
	string Load();
	void   SetProxy(string host,int port);
	void   SetUrl(string url);
	void   AddParam(string key,string value);
	string GetHttpGetRequest(string url);
	string GetHttpPostRequest(string url);
	string GetHost(string url);
	int    GetPort(string url);
	string GetPath(string url);
private:
	string LoadUrl(string url,string param);
	string SaveUrl(string url,string param);
	string WriteAndRead(string host,int port,string data);
private:
	string m_proxyHost;
	int    m_proxyPort;
	string m_url;
	map<string,string> m_params;
};
