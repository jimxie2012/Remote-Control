#ifndef CHXDB_H
#define CHXDB_H
#include "SHDbRes.h"

#ifdef WIN32
class __declspec(dllexport) CSHMySql
#endif
#ifdef _LINUX
class CSHMySql
#endif
{
public: 
	CSHMySql();
	~CSHMySql();
public:  
	static unsigned int Atoul(const char *data);
public:  
    bool            Delete(unsigned int id);  
    bool            PingDb();
    bool            CopyTable(const char *newName,const char *oldName);
    bool            DropTable(const char *table);
    bool            Open(string tables); 
    bool            Add(); 
    bool            Load(unsigned int id);
    bool            Load(const char* sql=NULL);
    bool            Next();
    bool            RunSql(const char *sql);
    bool            Update(unsigned int id);
    bool            CloseRes();
    bool            Close(); 
    void            CloseAll();
    bool            Connect(const char *server="db2.cnetbbs.com",
	                    const char *db="hxdb",
			    int         port=3306,
			    const char *usr="root",
			    const char *pass="njclever123");
public:
   string           m_tableName;
   string           m_errMsg;
private:
   void             *m_conn;
   void             *m_res;
   string            m_server;
   string            m_db;
   int               m_port;
   string            m_user;
   string            m_pass;
public:
    CSHDbRes        m_dataSet;
public:
	unsigned int    m_id;
};

#endif
