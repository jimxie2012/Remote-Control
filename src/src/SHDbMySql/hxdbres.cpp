#include "StdAfx.h"
#include <mysql.h>
#include "SHDbRes.h"
#include <string.h>
#include <stdio.h>

CSHDbRes::CHXDbCol::CHXDbCol()
{
	m_id = 0;         //col ID
    memset(m_name,0,sizeof(m_name));   //col Name
    m_type = 0 ;       //col Type
    m_data = "";       //col data
    m_data1= 0;
}
CSHDbRes::CHXDbCol::~CHXDbCol()
{
}

CSHDbRes::CSHDbRes()
{
  Init();
}
CSHDbRes::~CSHDbRes()
{
}
bool CSHDbRes::Init()
{
   m_colNum = 0 ;
   return true;
}
void CSHDbRes::AddCol(int type,const char *name)
{
   m_cols[m_colNum].m_id = m_colNum;
   strcpy(m_cols[m_colNum].m_name,name);
   m_cols[m_colNum].m_type = (enum enum_field_types)type;

   m_colNum++;
}
void CSHDbRes::AddData(int colId,string data)
{
   if (colId >= m_colNum)
      return ;
   if ( colId < 0 )
      return ;
   m_cols[colId].m_data = data;
}
string CSHDbRes::MakeLower(string a)
{
 for (int i=0; i<a.length(); i++) {
  a[i] = tolower(a[i]);
 }
 return a;
}
bool CSHDbRes::AddData(string colName,string data)
{
   int pos = -1;
   for ( int i = 0 ; i < m_colNum ; i ++ )
   {
	   string t1 = MakeLower(colName);
	   string t2 = MakeLower(m_cols[i].m_name);
     if ( t1 == t2 )
     {
       pos = i ;
       break;
     }
   }
   if ( pos < 0 )
      return false;
   if (pos >= m_colNum)
      return false;
   m_cols[pos].m_data = data;
   return true;
}

string CSHDbRes::GetData(const char *col)
{
   int pos = -1;
   for ( int i = 0 ; i < m_colNum ; i ++ )
   {	
	   string t1 = MakeLower(col);
	   string t2 = MakeLower(m_cols[i].m_name);
     if ( t1 == t2 )
     {
       pos = i ;
       break;
     }
   }
   if ( pos < 0 )
      return "";
   return m_cols[pos].m_data;
}
int CSHDbRes::GetColAccount()
{
  return  m_colNum;
}
void CSHDbRes::Print()
{
  for ( int i = 0 ;i < GetColAccount() ; i ++ )
  {
    printf("id   = %d \r\n",m_cols[i].m_id);
    printf("type = %d \r\n",m_cols[i].m_type);
    printf("name= %s \r\n",m_cols[i].m_name);
    printf("data = %s \r\n",m_cols[i].m_data.c_str());    
  }
  printf("\r\n\r\n");  
}
void CSHDbRes::AddData(const char *colName,unsigned int data)
{
   int pos = -1;
   for ( int i = 0 ; i < m_colNum ; i ++ )
   {
	   string t1 = MakeLower(colName);
	   string t2 = MakeLower(m_cols[i].m_name);
     if ( t1 == t2 )
     {
       pos = i ;
       break;
     }
   }
   if ( pos < 0 )
      return ;
   if (pos >= m_colNum)
      return ;
   char buf[100] = { 0 };
   sprintf(buf,"%u",data);
   m_cols[pos].m_data = buf;
   m_cols[pos].m_data1 = data;
}
