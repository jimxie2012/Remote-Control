#pragma once
#ifndef _SHSESSION_DATA	
#define _SHSESSION_DATA 	
#include <iostream>
#include <string.h>
#include <Common.h>
#include "SHAio.h"

#define SH_AGENT_ID  int
#define SH_SOCKET    int

using namespace std;

#define   SH_SOFT_VAR             5    //var of protocol
#define   SH_NOOP                 1    //Pulse Action 
#define   CMD_CONNECT             2    //Ask v-server connect remote
#define   CMD_CONNECT_ACK         3    //answer to v-client with Connect Result
#define   CMD_CLOSE               4    //ask server to close the connect
#define   SH_SET_PORT             5    //set pulse port
#define   SH_DETECT               6    //detect port is alive

class SH_EXPORT CSHDataHead{
public:
   typedef struct st_head{      
        int           m_var;      
	int           m_cmd;     
	bool          m_flag;   
        char          m_remoteHost[256];
        int           m_remotePort;
        char          m_proxyHost[256];
        int           m_proxyPort;
        bool          m_isTcp;
        SH_HANDLE     m_session;    
	int           m_key;
   }ST_HEAD;
public:
   CSHDataHead(const char *buf);
   CSHDataHead(int cmd);
   ~CSHDataHead(void);	
public:
   static int     GetSize();
public:
   int           GetKey();
   void          SetKey(int key);
   void           SetCmd(int cmd);
   int            GetCmd();
   bool           GetFlag();
   void           SetFlag(bool flag);
   SH_HANDLE      GetSession();
   void           SetSession(SH_HANDLE id);
   string         GetRemoteHost();
   int            GetRemotePort();
   string         GetProxyHost();
   int            GetProxyPort();
   void           SetProxy(const char *host,int port);
   void           SetRemote(const char *host,int port);
   void          *GetData();
   void           SetData(char *data,int size);
   string         GetString();
   bool           SetString(string str);
private:
   ST_HEAD        m_head;
};
class SH_EXPORT CSHDataPulse
{
public:
	typedef struct st_data
	{          	
		int              m_var;
		int              m_cmd;
		SH_AGENT_ID      m_id; 
		char             m_acl[16];	     ///RVSN
		char             m_agentIp[32];
		int              m_agentPort;
		char             m_proxyHost[128];
		int              m_proxyPort;
		char             m_privateIp[32];
		char             m_user[64];
		struct tm        m_activeTime;
	}ST_DATA;
public:
	CSHDataPulse();
	~CSHDataPulse(); 
public:	
	int            GetSize();
	char *         GetData();
	void           SetCmd(int  cmd);
	int            GetCmd();
	const char *   GetProxyHost();
	int            GetProxyPort();
	void           SetProxyAddr(const char *host,int port);
	const char *   GetAgentIp();
	int            GetAgentPort();
	void           SetAgentAddr(const char *ip,int port);
	void           SetId(SH_AGENT_ID idSocket);
    SH_AGENT_ID    GetId();
	void           SetPrivate(const char *privateIp,const char *admin_user);
	const char *   GetPrivateIp();
	const char *   GetAdminUser();
	const char *   GetAcl();
	void           SetAcl(const char *acl);
    void           SetData(char *data,int size);
	string         GetString();
	bool           SetString(string str);
private:
    ST_DATA        m_data;
};
class SH_EXPORT CSHDataConnect
{
public:
	typedef struct st_head
	{      
		int           m_var;      
		int           m_cmd;     
		bool          m_flag;   
 	    char          m_remoteHost[256];
	    int           m_remotePort;
		bool          m_isTcp;
		SH_HANDLE     m_agentId;  
		char          m_user[128];	 
		char          m_pass[128];	
		int           m_maxLockTime;
		int           m_responsePort;
	}ST_HEAD;
public:
	CSHDataConnect(const char *buf);
	CSHDataConnect();
	~CSHDataConnect(void);	
public:
	static int     GetSize();
public:
	int            GetCmd();
	bool           GetFlag();
	void           SetFlag(bool flag);
	SH_HANDLE      GetAgentId();
    string         GetRemoteHost();
    int            GetRemotePort();
	void           SetUser(int maxLockTime,const char  *userName,const char *password);
    void           SetRemote(SH_HANDLE agentId,const char *host,int port);
	void          *GetData();
	void           SetResponsePort(int port);
	int            GetResponsePort();
	int            GetMaxLockTime();
	string         GetUser();
	string         GetPass();  
    void           SetData(char *data,int size);
	//string         GetString();
	//bool           SetString(string str);
private:
    ST_HEAD        m_head;
};
#endif		
