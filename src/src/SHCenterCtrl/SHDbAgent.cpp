#include "SHDbAgent.h"
#include "SHLibCommon.h"
#include <stdio.h>
#include <SHConfig.h>
int g_max_offline_second = 180;

CSHDbAgent::CSHDbAgent(){
}
CSHDbAgent::~CSHDbAgent(){
}
void CSHDbAgent::UpdateData(bool isRead){
    if ( isRead ){
        m_id              = Atoul(m_dataSet.GetData("id").c_str());
        m_agent_name      = m_dataSet.GetData("agent_name");
        m_agent_user      = m_dataSet.GetData("agent_user");
        m_agent_desc      = m_dataSet.GetData("agent_desc");
        m_private_host    = m_dataSet.GetData("private_host");
        m_private_ip      = m_dataSet.GetData("private_ip");
        m_agent_ip        = m_dataSet.GetData("agent_ip");
        m_agent_port      = Atoul(m_dataSet.GetData("agent_port").c_str());
        m_proxy_host      = m_dataSet.GetData("proxy_host");
        m_proxy_port      = Atoul(m_dataSet.GetData("proxy_port").c_str());
        m_agent_pass      = m_dataSet.GetData("agent_pass");
        m_active_time     = m_dataSet.GetData("active_time");
        m_agent_stats     = Atoul(m_dataSet.GetData("agent_stats").c_str());
        m_agent_id        = Atoul(m_dataSet.GetData("agent_id").c_str());
    }
    else{
	m_dataSet.AddData("id",m_id);
        m_dataSet.AddData("agent_name",m_agent_name);
        m_dataSet.AddData("agent_user",m_agent_user);
	m_dataSet.AddData("agent_desc",m_agent_desc);
	m_dataSet.AddData("private_host",m_private_host);
        m_dataSet.AddData("private_ip",m_private_ip);
        m_dataSet.AddData("agent_ip",m_agent_ip);
        m_dataSet.AddData("agent_port",m_agent_port);
        m_dataSet.AddData("proxy_host",m_proxy_host);
        m_dataSet.AddData("proxy_port",m_proxy_port);
        m_dataSet.AddData("agent_pass",m_agent_pass);
        m_dataSet.AddData("active_time",m_active_time);
        m_dataSet.AddData("agent_stats",m_agent_stats);
        m_dataSet.AddData("agent_id",m_agent_id);
    }
}
bool CSHDbAgent::Add(){
    m_id = 0 ;
    UpdateData(false);
    return CSHMySql::Add();
}
bool CSHDbAgent::Update(unsigned int id){
    m_id = id;
    UpdateData(false);
    return CSHMySql::Update(id);
}
bool CSHDbAgent::Load(unsigned int agent_id){
     bool ret = false;
     char sql[100] = { 0 };
     sprintf(sql,"select * from %s where agent_id = %ld ",m_tableName.c_str(),agent_id);
     if ( CSHMySql::Load(sql)){
         UpdateData(true);
	 ret = true;
     }
     return ret;
}
bool CSHDbAgent::Connect(){
        
	bool ret = CSHMySql::Connect(CSHConfig::GetDbHost().c_str(),
                                     "hxdb",
                                     CSHConfig::GetDbPort(),
                                     "root",
                                     CSHConfig::GetDbPass().c_str());
	if ( ret )
	   ret = Open("db_agent");
        else
           printf("can not connect to db localhost\n");
        if ( ret )
            ret = m_user.Connect();
	return ret;
}
//SELECT  active_time,NOW(),UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(active_time) FROM `db_agent`;
bool CSHDbAgent::SetOffLine(){
        char sql[1024];
        int offSeconds = g_max_offline_second;
        sprintf(sql,"update %s set agent_stats = 0 where (UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(active_time)) > %d ", m_tableName.c_str(),offSeconds);
        printf("run sql %s\n",sql);
        return RunSql(sql);
}
bool CSHDbAgent::DeleteTimeOutWebSession(){
        char sql[1024];
        int timeOutSeconds = 7200;
        sprintf(sql,"delete from db_session where (UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(createTime)) > %d ", timeOutSeconds);
        printf("run sql %s\n",sql);
        return RunSql(sql);
}
bool CSHDbAgent::SetPulse(CSHDataPulse::ST_DATA data){	
	int var = data.m_var;
        unsigned int id = data.m_id;
        if ( this->Load(id) ){
            unsigned int dbid = this->m_id;
            /*
            m_agent_id = data.m_id;
            m_agent_pass = data.m_acl;
            m_agent_ip   = data.m_agentIp;
            m_agent_port = data.m_agentPort;
            m_proxy_host = data.m_proxyHost;
            m_proxy_port = data.m_proxyPort;
            m_private_ip = data.m_privateIp;
            m_private_host = "";
            m_active_time  = CSHCommon::GetTimeString(data.m_activeTime);
            m_agent_user   = data.m_user;
            m_agent_stats  = 1;
            return this->Update(dbid);
            */
            char sql[1024];
            sprintf(sql,"update %s set agent_stats=1,agent_pass='%s',agent_ip='%s',agent_port=%d,proxy_host='%s',proxy_port=%d,private_ip='%s',active_time='%s',agent_user='%s' where agent_id=%ld",m_tableName.c_str(),data.m_acl,data.m_agentIp,data.m_agentPort,data.m_proxyHost,data.m_proxyPort,data.m_privateIp,CSHCommon::GetTimeString(data.m_activeTime).c_str(),data.m_user,data.m_id);
        printf("run sql %s\n",sql);
        return RunSql(sql);
        }
        else{
            struct tm dbNow = CSHCommon::GetCurrentDateTime();
            if ( CSHCommon::GetSeconds(dbNow,data.m_activeTime ) <= g_max_offline_second ) {
                unsigned int dbid = this->m_id;
                m_agent_id = data.m_id;
                m_agent_pass = data.m_acl;
                m_agent_ip   = data.m_agentIp;
                m_agent_port = data.m_agentPort;
                m_proxy_host = data.m_proxyHost;
                m_proxy_port = data.m_proxyPort;
                m_private_ip = data.m_privateIp;
                m_private_host = "";
                m_active_time  = CSHCommon::GetTimeString(data.m_activeTime);
                m_agent_user   = data.m_user;
                m_agent_stats  = 1;
                char tmp[1024] = {0};
                sprintf(tmp,"%d",m_agent_id);
                if ( m_user.Load(data.m_id) )
                    strcpy(tmp,m_user.m_user_name.c_str());
                m_agent_name   = tmp;
                m_agent_desc   = "";
                //printf("begin add %u,%u\n",m_agent_id,dbid);
                return this->Add();
            }
            else{
                return false;
            }
        }
}
