#include "SHDbUser.h"
#include "SHLibCommon.h"
#include <stdio.h>
#include <SHConfig.h>

CSHDbUser::CSHDbUser(){
}
CSHDbUser::~CSHDbUser(){
}
void CSHDbUser::UpdateData(bool isRead){
    if ( isRead ){
        m_id              = Atoul(m_dataSet.GetData("id").c_str());
        m_user_name      = m_dataSet.GetData("username");
    }
    else{
	m_dataSet.AddData("id",m_id);
        m_dataSet.AddData("username",m_user_name);
    }
}
bool CSHDbUser::Load(unsigned int agent_id){
     bool ret = false;
     char sql[100] = { 0 };
     sprintf(sql,"select * from %s where id = %ld ",m_tableName.c_str(),agent_id);
     if ( CSHMySql::Load(sql)){
         UpdateData(true);
	 ret = true;
     }
     return ret;
}
bool CSHDbUser::Connect(){
	bool ret = CSHMySql::Connect(CSHConfig::GetDbHost().c_str(),
                                     "hxdb",
                                     CSHConfig::GetDbPort(),
                                     "root",
                                     CSHConfig::GetDbPass().c_str());
	if ( ret )
	   ret = Open("db_admin_user");
        else
           printf("can not connect to db localhost\n");
	return ret;
}
