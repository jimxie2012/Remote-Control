#ifndef CSHDbRes_H
#define CSHDbRes_H

#include <string>
using std::string;
#ifdef WIN32
class __declspec(dllexport) CSHDbRes
#endif
#ifdef _LINUX
class CSHDbRes
#endif
{
private:
    #ifdef WIN32
    class __declspec(dllexport) CHXDbCol
    #endif
    #ifdef _LINUX
	class CHXDbCol
    #endif
	{
	public:
		CHXDbCol();
    	~CHXDbCol();
    public:
		int                  m_id;         //col ID
        char                 m_name[60];   //col Name
        int                  m_type;       //col Type
        string               m_data;       //col data
        unsigned int         m_data1;
	};        
public: 
	CSHDbRes();
	~CSHDbRes();
public:
  bool     Init();
  void     AddData(int colId,string data);
  void     AddCol(int type,const char *name);
  string   GetData(const char *col);
  int      GetColAccount();
  bool     AddData(string colName,string data);
  void     AddData(const char *colName,unsigned int data); 
  void     Print();
public:
  CHXDbCol    m_cols[50];
  int         m_colNum;
public:
  static string    MakeLower(string a); 
};

#endif
