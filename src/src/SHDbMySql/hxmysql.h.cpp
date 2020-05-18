#include "StdAfx.h"
#include <mysql.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <exception>
#include <stdlib.h>
#include <iostream>
#include <SHMySql.h>

CSHMySql::CSHMySql()
{
   m_res = NULL ;
   m_conn = new MYSQL;
   if ( mysql_init(( MYSQL *)m_conn) == NULL )
   {
      printf("init mysql error\r\n");
   }
   m_errMsg = "";
   m_id = 0;
}
CSHMySql::~CSHMySql(){
   delete (MYSQL *)m_conn;
}
bool CSHMySql::Connect(const char *server,const char *db,int port ,const char *usr,const char *pass)
{   
   m_server = server;
   m_db     = db;
   m_port   = port;
   m_user   = usr;
   m_pass   = pass;
   m_res = NULL ; 
   
   char value = 1;
   mysql_options(( MYSQL *)m_conn, MYSQL_OPT_RECONNECT, (char *)&value);

   if (!mysql_real_connect(( MYSQL *)m_conn,server,usr,pass,db,port,NULL,0)){
      mysql_query(( MYSQL *)m_conn,"set names ’utf8’ ");
      mysql_query(( MYSQL *)m_conn,"set character_set_client=utf8");
      mysql_query(( MYSQL *)m_conn,"set character_set_results=utf8");
      return false;
   }
   return true;
}
bool CSHMySql::Load(const char* sql)
{
   CloseRes();
   string test;
   if ( sql == NULL )
       test = "select * from "+ m_tableName;
   else
       test = sql;
   if ( mysql_query(( MYSQL *)m_conn,test.c_str()) )
   {
     return false;
   }
   m_res = mysql_use_result(( MYSQL *)m_conn);
   if ( (MYSQL_RES    *)m_res == NULL )
     return false;
               
   if ( mysql_num_fields((MYSQL_RES    *)m_res) <= 0 )
     return false;
   m_dataSet.Init();
   MYSQL_FIELD  *fd = NULL;
   for ( int i = 0 ; fd=mysql_fetch_field((MYSQL_RES    *)m_res);i++)
   {
     m_dataSet.AddCol(fd->type,fd->name);
   }
   return Next();
}
bool CSHMySql::Next()
{
    if ( (MYSQL_RES    *)m_res == NULL )
       return false;     
    MYSQL_ROW   row;
    row = mysql_fetch_row((MYSQL_RES    *)m_res);
    if ( row == NULL )
       return false;
    for ( int i = 0 ; i < m_dataSet.GetColAccount() ; i ++ )
    {
      if ( row[i] == NULL )
      {
         m_dataSet.AddData(i,"");
         continue;       
      }
      string tmp = row[i];
      if (tmp =="")
        m_dataSet.AddData(i,"");
      else
        m_dataSet.AddData(i,tmp);
    }
    return true;
}
bool CSHMySql::Close()
{
   try{
       if ( m_conn != NULL ){
           mysql_close(( MYSQL *)m_conn);
       }
   }
   catch(...){
       printf("Close SHMySql exception\n");
   }  
   return true;
}
bool CSHMySql::RunSql(const char *sql)
{
   CloseRes();
   if ( mysql_query(( MYSQL *)m_conn,sql) )
   {
     CloseRes();
     return false; 
   }
   CloseRes();
   return true;
}
bool CSHMySql::CloseRes()
{
   try{
       if ((MYSQL_RES    *)m_res != NULL ){
           mysql_free_result((MYSQL_RES    *)m_res);
           m_res = NULL;
       }
   }
   catch(...){
       printf("CloseRes SHMySql exception\n");
   }
   return true;
}
bool CSHMySql::Open(string tables)
{
   m_tableName = tables;
   m_dataSet.Init();
   string sql = "select * from ";
   sql = sql + tables;
   sql = sql + " limit 1";

   if ( mysql_query(( MYSQL *)m_conn,sql.c_str()) )
   {
     return false;
   }
   m_res = mysql_use_result(( MYSQL *)m_conn);
   if ( m_res == NULL )
     return false;
   if ( mysql_num_fields((MYSQL_RES    *)m_res) <= 0 )
     return false;
   MYSQL_FIELD  *fd = NULL;
   for ( int i = 0 ; fd=mysql_fetch_field((MYSQL_RES    *)m_res);i++)
   {
     m_dataSet.AddCol(fd->type,fd->name);
   }
   return CloseRes();
}
bool CSHMySql::Update(unsigned int id)
{
	CloseRes();
   m_id = id;
   MYSQL_STMT   *stmt;
   MYSQL_BIND bind[1024];
   string sql = "";
   string cols = "";
   memset(bind,0,sizeof(bind));		 
   char tempId[100]= { 0 };
   for ( int i = 0 ; i < m_dataSet.m_colNum ; i ++ )
   {	
	   bind[i].buffer_type  = (enum enum_field_types)(m_dataSet.m_cols[i].m_type);
	   string colId = CSHDbRes::MakeLower(m_dataSet.m_cols[i].m_name);
	   if ( colId == "hxid" )
	   {
		   sprintf(tempId,"%ld",id);
		   bind[i].buffer_type   =  MYSQL_TYPE_VARCHAR;
           bind[i].buffer_length =  strlen(tempId);
           bind[i].buffer        =  tempId;
	   }
	   else
	   {
		   if (( MYSQL_TYPE_VARCHAR <= bind[i].buffer_type) && (bind[i].buffer_type <=  MYSQL_TYPE_GEOMETRY ))
		   {
			   bind[i].buffer_length = m_dataSet.m_cols[i].m_data.size();
               bind[i].buffer      = (char *)m_dataSet.m_cols[i].m_data.c_str();
		   }
		   else
		   {
			   bind[i].buffer_type   =  MYSQL_TYPE_VARCHAR;
               bind[i].buffer_length = m_dataSet.m_cols[i].m_data.size();
               bind[i].buffer        = (char *)m_dataSet.m_cols[i].m_data.c_str();
		   }
	   }
      if ( i >0 )
         cols = cols +", ";
      cols = cols + m_dataSet.m_cols[i].m_name;
	  cols = cols + "=? ";
   }
   sql =  "update ";
   sql = sql + m_tableName;
   sql = sql + " set ";
   sql = sql + cols;
   char tmp[100] = { 0 };
   sprintf(tmp," where id=%ld ",id);
   sql = sql + tmp;
   stmt = mysql_stmt_init(( MYSQL *)m_conn);
   if ( mysql_stmt_prepare(stmt,sql.c_str(),strlen(sql.c_str())) !=0 )
   {	
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }
   mysql_stmt_param_count(stmt);

   if ( mysql_stmt_bind_param(stmt,bind) != 0 )
   {
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }
   if ( mysql_stmt_execute(stmt) != 0 )
   {
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }
   mysql_stmt_close(stmt);    
   return CloseRes();;
}

bool CSHMySql::Add()
{
	m_id = 0;
	CloseRes();
   MYSQL_STMT   *stmt;
   MYSQL_BIND bind[1024];
   string sql = "";
   string cols = "";
   string datas = "";
   memset(bind,0,sizeof(bind));
   for ( int i = 0 ; i < m_dataSet.m_colNum ; i ++ )
   {
      bind[i].buffer_type  = (enum enum_field_types)(m_dataSet.m_cols[i].m_type);
      if (( MYSQL_TYPE_VARCHAR <= bind[i].buffer_type) && (bind[i].buffer_type <=  MYSQL_TYPE_GEOMETRY ))
      {
          bind[i].buffer_length = m_dataSet.m_cols[i].m_data.size();
          bind[i].buffer      = (char *)m_dataSet.m_cols[i].m_data.c_str();
      }
      else
      {
          bind[i].buffer_type   =  MYSQL_TYPE_VARCHAR;
          bind[i].buffer_length = m_dataSet.m_cols[i].m_data.size();
          bind[i].buffer      = (char *)m_dataSet.m_cols[i].m_data.c_str();
      }
      if ( i >0 )
         cols = cols +",";
      cols = cols + m_dataSet.m_cols[i].m_name;
      if ( i >0 )
         datas = datas +",";
      datas = datas +"?";
   }
   sql =  "insert into ";
   sql = sql + m_tableName;
   sql = sql + " ( ";
   sql = sql + cols;
   sql = sql + " ) values ( ";
   sql = sql + datas;
   sql = sql + " ) ";
   //printf("sql %s \r\n",sql.c_str());
   stmt = mysql_stmt_init(( MYSQL *)m_conn);
   if ( mysql_stmt_prepare(stmt,sql.c_str(),strlen(sql.c_str())) !=0 )
   {
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }
   mysql_stmt_param_count(stmt);

   if ( mysql_stmt_bind_param(stmt,bind) != 0 )
   {
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }
   if ( mysql_stmt_execute(stmt) != 0 )
   {
	   m_errMsg = mysql_error(( MYSQL *)m_conn);
     return false;
   }

   m_id = mysql_insert_id(( MYSQL *)m_conn);  
   mysql_stmt_close(stmt);
   return 	CloseRes();;
}
bool CSHMySql::Delete(unsigned int id)
{
	m_id = id;
   string sql = "delete from ";
   sql = sql + m_tableName;
   sql = sql +" where id=";
   char buf[100]={0};
   sprintf(buf,"%u",id);
   sql = sql + buf;
   return RunSql(sql.c_str());
}
bool CSHMySql::Load(unsigned int id)
{
   string sql = "select * from ";
   sql = sql + m_tableName;
   sql = sql +" where id=";
   char buf[100]={0};
   sprintf(buf,"%ld",id);
   sql = sql + buf;
   return Load(sql.c_str());
}
void CSHMySql::CloseAll()
{
   CloseRes();
   Close();
}
bool CSHMySql::PingDb()
{
	if ( mysql_ping(( MYSQL *)m_conn) == 0 )
		return true;
	else
	   return false;
}
bool CSHMySql::CopyTable(const char *newName,const char *oldName)
{
  char sql[1024] = { 0 };
  sprintf(sql,"create table %s (id int(20) unsigned not null auto_increment primary key) select * from %s where 1=2",newName,oldName);     
  return RunSql(sql);
}
bool CSHMySql::DropTable(const char *table)
{
  char sql[1024] = { 0 };
  sprintf(sql,"DROP table %s ",table);
  return RunSql(sql);
}
unsigned int CSHMySql::Atoul(const char *data)
{
   if (data == NULL )
      return 0;
   if (strcmp(data,"") == 0 )
      return 0;
   return   strtoul(data,NULL,10);
}
