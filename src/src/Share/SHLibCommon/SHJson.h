#pragma once
#include "Common.h"
#include <string>
using namespace std;
/*
use samples:
    CSHJson root(true);
	root.Add("id",1002);
	root.Add("name","jim");
	CSHJson dataArray(false);
	dataArray.Add(1);
	dataArray.Add(2);
	dataArray.Add("hello word");
	dataArray.Add(root);
	root.Add("dataArray",dataArray);
	printf("%s\n",root.GetString().c_str());
	printf("%d\n",root.GetSize());
	printf("%s\n",root.GetData("id").c_str());
	printf("%s\n",root.GetData("name").c_str());
	CSHJson temp;
	root.GetItem("dataArray",temp);
	printf("%s\n",temp.GetString().c_str());
*/
class SH_EXPORT CSHJson
{
public:
	CSHJson(bool isDic);
	CSHJson(string str);
	CSHJson(char *str);
	CSHJson(void);
	~CSHJson(void);
public:
	///only for array
	bool  Add(CSHJson &item);
	bool  Add(double data);
	bool  Add(string data);
	///only for dic
	bool  Add(string key,CSHJson &item);
	bool  Add(string key,double data);
	bool  Add(string key,string data);
	///for dic or array
	int    GetSize();
	string GetKey(int item);
	string GetData(string key);
	bool   GetItem(string key,CSHJson &subItem);
public:
	string GetString();
	bool   ParseString(string str);
	bool   IsArray();
	bool   IsDic();
private:	
	void *CreateArray();
	void *CreateObject();
private:
	void *m_root;
};

