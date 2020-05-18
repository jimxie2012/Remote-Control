#pragma once
#include "Common.h"
#include "SHDataApp.h"
#include <SHAio.h>

class SH_EXPORT CSHConfig{
public:
   CSHConfig(string role);
   ~CSHConfig();
public:
    bool Load();
    bool Save();
public:
	static int    GetVersion();
	static void   SetVersion(int ver);
    static string GetDbHost();
    static int    GetDbPort();
    static string GetDbPass();
    static string GetRestfulUrl();
	static bool   GetAutoRun();
	static void   SetAutoRun(bool autoRun);
public:
   bool          m_withLog;
   unsigned int  m_logLevel;
   int           m_dataPort;
   int           m_pulsePort1;
   int           m_pulsePort2;
   int           m_pulsePort3;
   int           m_maxAio;
   int           m_checkTimer;
   int           m_timeOut;
   int           m_threadAccount;

   SH_HANDLE     m_agentId;
   string        m_agentAcl;
   string        m_proxyHost;
   int           m_sleepTime;

   int           m_vServerPort;
   vector<int>   m_vServerList;
   int           m_maxLockTime;

   string        m_agent_admin;
   bool          m_savePass;
   bool          m_autoLogIn;
   string        m_agent_user;
   string        m_agent_user_pass;

   //for proxy
   bool          m_isRestProxy;
   string        m_restProxyHost;
   int           m_restProxyPort;
private:
   string        m_fileName;
   string        m_sectionName;
private:
   string        ReadString(const char *szKey);
   bool          ReadBool(const char *szKey);
   int           ReadInt(const char *szKey);
   unsigned int  ReadUInt(const char *szKey);
   bool          WriteString(const char *szKey,string data);
   bool          WriteBool(const char *szKey,bool data);
   bool          WriteInt(const char *szKey,int data);
   bool          WriteUInt(const char *szKey,unsigned int data);
};
