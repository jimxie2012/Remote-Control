#include "SHJson.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

CSHJson::CSHJson(void){
	m_root = NULL;
}
CSHJson::CSHJson(bool isDic){	
	if ( isDic )
		m_root = CreateObject();
	else
		m_root = CreateArray();
}
CSHJson::CSHJson(string str){	
	ParseString(str.c_str());
}
CSHJson::CSHJson(char *str){	
	ParseString(str);
}
bool CSHJson::IsArray(){
	return (((cJSON *)m_root)->type == cJSON_Array);
}
bool CSHJson::IsDic(){
	return (((cJSON *)m_root)->type == cJSON_Object);
}
bool CSHJson::ParseString(string str){
	m_root = cJSON_Parse(str.c_str());
	return (m_root != NULL);
}
CSHJson::~CSHJson(void){
	if ( m_root != NULL )
		cJSON_Delete((cJSON *)m_root);
}
void *CSHJson::CreateArray(){
	return cJSON_CreateArray();
}
void *CSHJson::CreateObject(){
	return cJSON_CreateObject();
}
bool CSHJson::Add(CSHJson &item){
	if(IsArray()){
		cJSON * temp = cJSON_Duplicate((cJSON *)(item.m_root),1);
		cJSON_AddItemToArray((cJSON *)m_root,temp);
		return true;
	}
	return false;
}
bool CSHJson::Add(string key,CSHJson &item){
	if(IsDic()){
		cJSON *temp = cJSON_Duplicate((cJSON *)(item.m_root),1);
		cJSON_AddItemToObject((cJSON *)m_root,key.c_str(),temp);
		return true;
	}
	return false;
}
bool  CSHJson::Add(double data){
	if( !IsArray() )
		return false;
	cJSON_AddItemToArray((cJSON *)m_root,cJSON_CreateNumber(data));
	return true;
}
bool  CSHJson::Add(string data){	
	if( !IsArray() )
		return false;
	cJSON_AddItemToArray((cJSON *)m_root,cJSON_CreateString(data.c_str()));
	return true;
}
bool  CSHJson::Add(string key,double data){	
	if( !IsDic() )
		return false;
	cJSON_AddNumberToObject((cJSON *)m_root,key.c_str(),data);	
	return true;
}
bool  CSHJson::Add(string key,string data){	
	if( !IsDic() )
		return false;
	cJSON_AddStringToObject((cJSON *)m_root,key.c_str(),data.c_str());
	return true;
}
string CSHJson::GetString(){
	char *out = cJSON_Print((cJSON *)m_root);
	string ret = out;
	free(out);
	return ret;
}
int CSHJson::GetSize(){
	if ( m_root == NULL )
		return 0;
	return cJSON_GetArraySize((cJSON *)m_root);
}
string CSHJson::GetKey(int item){
	cJSON *test = cJSON_GetArrayItem((cJSON *)m_root,item);
	if ( test == NULL )
		return NULL;
	if (test->type == cJSON_String )
		return test->valuestring;	
	if (test->type == cJSON_Number ){
	    char ret[10] = { 0 };
		sprintf(ret,"%f",test->valuedouble);
		return ret;
	}	
	if (test->type == cJSON_Object ){
		char *out = cJSON_Print((cJSON *)test);
	    string ret = out;
	    free(out);
	    return ret;
	}
	return test->string;
}
string CSHJson::GetData(string key){
	cJSON *test = cJSON_GetObjectItem((cJSON *)m_root,key.c_str());
	if ( test == NULL )
		return "";
	char ret[10] = { 0 };
	if (test->type == cJSON_String )
		return test->valuestring;
	if (test->type == cJSON_Number ){
		sprintf(ret,"%f",test->valuedouble);
		return ret;
	}
	return "";
}
bool CSHJson::GetItem(string key,CSHJson &subItem){
	if ( subItem.m_root != NULL ){
		cJSON_Delete((cJSON *)(cJSON *)(subItem.m_root));
		subItem.m_root = NULL;
	}
	cJSON *temp= cJSON_GetObjectItem((cJSON *)m_root,key.c_str());
	if ( temp == NULL )
		return false;
	subItem.m_root = cJSON_Duplicate((cJSON *)(temp),1);
	return ( subItem.m_root != NULL );
}
