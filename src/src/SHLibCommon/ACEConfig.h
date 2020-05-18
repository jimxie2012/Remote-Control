/*
配置文件操作类
*/
#pragma once
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"

class CConfigTool 
{
public:
	int SetString(const char* szSection,const char* szKey,ACE_TString& strValue);
    int GetString(const char* szSection,const char* szKey,ACE_TString& strValue);
    int GetInteger(const char* szSection,const char* szKey,int& nValue);
    int open(const ACE_TCHAR * filename);
    int close();
    CConfigTool();
    ~CConfigTool();
private:
    ACE_Configuration_Section_Key root_key_;
    ACE_Ini_ImpExp * impExp_;
    ACE_Configuration_Heap config;
	ACE_TString m_fileName;
};
CConfigTool::CConfigTool():impExp_(NULL)
{
}
CConfigTool::~CConfigTool()
{
     if (impExp_) 
        delete impExp_;
     impExp_ = NULL;
} 
int CConfigTool::open(const ACE_TCHAR * filename)
{
	m_fileName = filename;
     if (this->config.open() == -1) 
        return -1;
     this->impExp_=new ACE_Ini_ImpExp(config);
     return this->impExp_->import_config(filename);
}
int CConfigTool::SetString(const char *szSection, const char *szKey, ACE_TString &strValue)
{
    if (config.open_section(config.root_section(),ACE_TEXT(szSection),1,this->root_key_)==-1)
       return -1;
    if( config.set_string_value(this->root_key_,szKey,strValue))
		return -1;
	return this->impExp_->export_config(m_fileName.c_str());
}
int CConfigTool::GetString(const char *szSection, const char *szKey, ACE_TString &strValue)
{
    if (config.open_section(config.root_section(),ACE_TEXT(szSection),0,this->root_key_)==-1)
       return -1;
    return config.get_string_value(this->root_key_,szKey,strValue);
}
int CConfigTool::GetInteger(const char* szSection,const char* szKey,int& nValue)
{
    ACE_TString strValue;
    if (config.open_section(config.root_section(),ACE_TEXT(szSection),0,this->root_key_)==-1)
        return -1;
    if (config.get_string_value(this->root_key_,szKey,strValue) == -1) 
       return -1;
    nValue = ACE_OS::atoi(strValue.c_str());
    if (nValue == 0 && strValue != "0") 
       return -1;
    return nValue;
}  
int CConfigTool::close()
{
   if (impExp_) 
   {
      delete impExp_;
      impExp_ = NULL;
   }
   return 0;
}

