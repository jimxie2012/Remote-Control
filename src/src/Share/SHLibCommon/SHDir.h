#pragma once
#include "Common.h"

class SH_EXPORT CSHDir
{
public:
	/**
    *在指定路径下新建一个目录；
    *@param path：[in]指定创建目录所在的路径；
    *@param directoryName：[in]指定创建目录的名称；
    *@return bool：是否创建成功；
    */
    static bool CreateDir(string path,const string directoryName);
    
    /**
    *在指定路径下删除一个目录；
    *@param path：[in]指定删除目录所在的路径；
    *@param directoryName：[in]指定删除目录的名称；
    *@return bool：是否删除成功；
    */
    static bool DelDir(string path,const string directoryName);
    
    /**
    *搜索指定路径path下的所有目录，并将所有目录名放入result中；
    *@param path：[in]指定搜索的路径；
    *@param result：[out]用来存放所有目录名称的vector<string>；
    *@return；
    */
    static void SearchDir(string path,vector<string> &result);
    
    /**
    *搜索指定路径path下的所有指定类型type的文件，并将所有该类型文件名放入result中；
    *@param path：指定搜索的路径；
    *@param type：指定搜索的文件类型；
    *@param result：用来存放所有文件名称的vector<string>；
    *@return；
    */
    static void SearchFile(string path,const string type,vector<string> &result);
    
    /**
    *查找路径path下是否有目录dirName的接口；
    *@param path：指定搜索的路径；
    *@param dirName：被查找的目录名称；
    *@return bool：找到了返回true，找不到返回false；
    */
    static bool FindADir(string path,const string dirName);

    /**
    *在指定路径下删除一个文件；
    *@param path：[in]指定删除文件所在的路径；
    *@param fileName：[in]指定删除文件的名称；
    *@return bool：是否删除成功；
    */
    static bool DelFile(string path,const string fileName);                
};