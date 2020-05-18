#include <iostream>
#include "SHDir.h"
using namespace std;

#ifdef WIN32

#include <windows.h>

/** 创建一个新目录的接口 */
bool CSHDir::CreateDir(string path,const string directoryName)
{
    /** 去掉路径最后可能有的"\" */
    if (path.at(path.length()-1)=='\\')
    {
       path.erase(path.length()-1,1); 
    }
    
    /** 字符串操作，拼成要创建的目录的全名 */
    string directoryFullName=path+"\\"+directoryName;
    
    /** 调用windows API: BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
        创建一个新的目录，并返回其值 */
    return (CreateDirectory(directoryFullName.c_str(),NULL)!=0);  

}
/** 删除一个目录的接口 */
bool CSHDir::DelDir(string path,const string directoryName)
{
    vector<string> dirResult,fileResult;
    vector<string>::iterator result_itr;
    
    /** 去掉路径最后可能有的"\" */
    if (path.at(path.length()-1)=='\\')
    {
       path.erase(path.length()-1,1); 
    }
    
    /** 字符串操作，拼成要删除的目录的全名 */
    string directoryFullName=path+"\\"+directoryName;
    
    /** 要删除的目录中的所有子目录名称放入dirResult中，要删除的目录中的所有文件名称放入fileResult中 */
    SearchDir(directoryFullName,dirResult);
    SearchFile(directoryFullName,"*.*",fileResult);
    
    /** 递归调用DeleteDir删除该目录中的所有的嵌套子目录 */
    for (result_itr=dirResult.begin(); result_itr!=dirResult.end();result_itr++)
        if((*result_itr!=".")&&(*result_itr!=".."))
            DelDir(directoryFullName,*result_itr);
        
    /** 再删除该目录中的所有文件 */
    for (result_itr=fileResult.begin(); result_itr!=fileResult.end();result_itr++)
        DelFile(directoryFullName,*result_itr);
        
    /** 调用windows API: BOOL RemoveDirectory(LPCTSTR lpPathName); 
        删除一个目录，并返回其值 */
    return (RemoveDirectory(directoryFullName.c_str())!=0);
}

/** 删除一个文件的接口 */
bool CSHDir::DelFile(string path,const string fileName)
{
       
    /** 去掉路径最后可能有的"\" */
    if (path.at(path.length()-1)=='\\')
    {
       path.erase(path.length()-1,1); 
    }
    
    /** 字符串操作，拼成要删除的文件的全名 */
    string fileFullName="\\\\?\\"+path+"\\"+fileName;
    
    /** 调用windows API: BOOL DeleteFile(LPCTSTR lpFileName ); 
        删除一个文件，并返回其值 */
    return (DeleteFile(fileFullName.c_str())!=0);
}

/** 查找一个目录的接口 */
void CSHDir::SearchDir(string path,vector<string> &result)
{
    /** 去掉路径最后可能有的"\" */
    if (path.at(path.length()-1)=='\\')
    {
       path.erase(path.length()-1,1); 
    }
    
    /** 查找第一个文件或目录 */
    string searchPath=path+"\\"+"*.*";
    WIN32_FIND_DATA wfd;                                  
    HANDLE hFind=FindFirstFile(searchPath.c_str(),&wfd);
    
    /** 如果当前路径下非空 */
    if (hFind!=INVALID_HANDLE_VALUE)
    {
        /** 首先初始化result，即将之清空 */
        result.clear();
        
        /** 如果是目录，则将其名称加入result中 */
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.push_back(wfd.cFileName);
            //cout<<wfd.cFileName<<endl;
        }/** end of if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) */
        
        /** 调用FindNextFile继续搜索当前路径下的所有文件或目录 */
        while (FindNextFile(hFind,&wfd))
		{
		   /** 如果是目录，则将其名称加入result中 */
		   if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		   {
		        result.push_back(wfd.cFileName);
		        //cout<<wfd.cFileName<<endl;
		   }/** end of if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) */
		   
		}/** end of while (FindNextFile(hFind,&wfd)) */
		
    }/** end of if (hFind!=INVALID_HANDLE_VALUE) */ 
    
    /** 搜索结束 */                                                                          
    FindClose(hFind);
    
    /** 将"."和".."从result中去掉 */
    vector<string>::iterator resultItr;
    for (resultItr=result.begin();resultItr!=result.end();resultItr++)
    {
        if (*resultItr==".")
            result.erase(resultItr);
    }
    
    if ( (result.size()==1) && (*result.begin()=="..") )
    /** result中只剩一个".."元素的处理方法 */
    {
        result.erase(result.begin());
    }
    else
    {
        for (resultItr=result.begin();resultItr!=result.end();resultItr++)
        {
            if (*resultItr=="..")
                result.erase(resultItr);
        }
    }
}

/** 查找一个文件的接口 */
void CSHDir::SearchFile(string path,string type,vector<string> &result)
{
    /** 去掉路径最后可能有的"\" */
    if (path.at(path.length()-1)=='\\')
    {
       path.erase(path.length()-1,1); 
    }
    
    /** 字符串操作，拼成要查找的文件路径和类型 */
    string fileFullName=path+"\\"+type;
    
    /** 查找第一个文件 */
    WIN32_FIND_DATA wfd;                                  
    HANDLE hFind=FindFirstFile(fileFullName.c_str(),&wfd);
    
    /** 如果找到了一个符合条件的文件，则将之名称放入result中 */
    if (hFind!=INVALID_HANDLE_VALUE)
    {
        /** 首先初始化result，即将之清空 */
        result.clear();
        
        /** 如果不是目录，则一定是文件,则将其名称加入result中 */
        if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            result.push_back(wfd.cFileName);
            //cout<<wfd.cFileName<<endl;
        }/** end of if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) */ 
        
        /** 调用FindNextFile继续搜索当前路径下的所有符合查找条件的文件或目录 */   
        while(FindNextFile(hFind,&wfd))
		{
		   /** 如果不是目录，则一定是文件,则将其名称加入result中 */
		   if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		   {
		        result.push_back(wfd.cFileName);
                //cout<<wfd.cFileName<<endl;
		   }/** end of if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) */
		   
		}/** end of while(FindNextFile(hFind,&wfd)) */

    }/** end of if (hFind!=INVALID_HANDLE_VALUE) */ 
    
    /** 搜索结束 */                                                                          
    FindClose(hFind);
}

/** 查找路径path下是否有目录dirName的接口 */
bool CSHDir::FindADir(string path,const string dirName)
{
    vector<string> result;
    vector<string>::iterator resultItr;
    
    /** 查找路径path下的所有目录，并将所有目录名放入result中 */
    SearchDir(path,result);
    
    /** 在结果中查找有无相同的目录名 */
    for(resultItr=result.begin();resultItr!=result.end();resultItr++)
    {
        /** 有该目录，则返回true */
        if (*resultItr==dirName) return true;
    }
    
    /** 无该目录，返回false */
    /** modify by wangwei at 20080124 */
    /// 到达该分支应当直接返回false
    ///if (resultItr==result.end())
    ///    return false;
    return false;
    /** modify by wangwei at 20080124  end */
}

#else 

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>

/** 创建一个新目录的接口 */
bool CSHDir::CreateDir(string path,const string directoryName)
{
	mode_t u_mode=0;
	mode_t new_mode=0777;
	
	/** 设置新目录的访问权限 **/
	umask(u_mode);
	
	/** 去掉路径最后可能有的"\" */
	
  if (path.at(path.length()-1)=='/')
	{
	   path.erase(path.length()-1,1); 
	}
	
	/** 字符串操作，拼成要创建的目录的全名 */
	string directoryFullName=path+"/"+directoryName;
	
	return (mkdir((const char*)directoryFullName.c_str(),new_mode));
}

/** 删除一个目录的接口 */
bool CSHDir::DelDir(string path,const string directoryName)
{
	vector<string> fileResult;		///存放所有查找到的子目录
	vector<string> dirResult;		///存放所有查找到的子文件
	vector<string>::iterator itr;
		
	///如果path的结尾没有'/',则补上
	if(path.at(path.length()-1)!='/')
	{
		path+="/";
	}

	path+=directoryName;

		
	///找出该目录下的所有文件并删除
	string filename;
	SearchFile(path,"*",fileResult);
	for(itr=fileResult.begin();itr!=fileResult.end();itr++)
	{
		filename = path + "/";
        filename += *itr;
        if(remove(filename.c_str())!=0)
		{
			return false;
		}
	}
	
	///递归输出目录下所有的子目录
	SearchDir(path,dirResult);
	for(itr=dirResult.begin();itr!=dirResult.end();itr++)
	{
		if(!DelDir(path,*itr))
		    return false;
	}
	
		
	if(remove(path.c_str())!=0)
    {
        return false;
    }
	return true;	
}

/** 删除一个文件的接口 */
bool CSHDir::DelFile(string path,const string fileName)
{
	string m_FileName;
	
	///查看路径后是否有'/',若没有则给其添加上
	if(path.at(path.length()-1)=='/')
	{
		m_FileName=path+fileName;
	}
	else
	{
		m_FileName=path+"/"+fileName;
	}
	
	return (remove((const char *)m_FileName.c_str())==0);
	
}
/** 查找一个文件的接口 */
void CSHDir::SearchFile(string path,string type,vector<string> &result)
{
	DIR * dp;					///目录指针
	struct dirent * dir;		///目录属性指针
	string m_FileName;			///文件名称
	struct stat statbuf;		///文件状态结构
	
	result.clear();
	///打开该目录
	if((dp=opendir((const char *)path.c_str()))==NULL)
	{
		return;
	}
	
	///若路径后无'/',给其添加上
	if(path.at(path.length()-1)!='/')
	{
		path+="/";
	}
	///处理每一个目录
	while((dir=readdir(dp))!=NULL)
	{
		m_FileName=path+dir->d_name;
		string dname=dir->d_name;
		
		///获取文件类型
		if(stat((const char *)m_FileName.c_str(),&statbuf)<0)
		{
			continue;
		}
		
		///只获取普通文件类型的文件
		if(S_ISREG(statbuf.st_mode))
		{
			if(fnmatch((const char *)type.c_str(),(const char *)m_FileName.c_str(),0)==0)
			{
				result.push_back(dname);
			}			
		}///endif S_IFREG(statbuf.st_mode)
				
	}///end while
	
	///关闭目录
	closedir(dp);
}

/** 查找一个目录的接口 */
void CSHDir::SearchDir(string path,vector<string> &result)
{
	DIR * dp;					///目录指针
	struct dirent * dir;		///目录属性指针
	string m_DirName;			///目录名称
	struct stat statbuf;		///文件状态结构
	
	result.clear();
	///打开该目录
	if((dp=opendir((const char *)path.c_str()))==NULL)
	{
		return;
	}
	
	///若路径后无'/',给其添加上
	if(path.at(path.length()-1)!='/')
	{
		path+="/";
	}
	///处理每一个目录
	while((dir=readdir(dp))!=NULL)
	{
		m_DirName=path+dir->d_name;
		string dname=dir->d_name;
		
		///获取文件类型
		if(stat((const char *)m_DirName.c_str(),&statbuf)<0)
		{
			continue;
		}
		
		if(S_ISDIR(statbuf.st_mode))
		{
			if((dname!=".") && (dname!=".."))
			{
				result.push_back(dname);
			}
		}
	}///end while
	
	closedir(dp);
}

/** 查找路径path下是否有目录dirName的接口 */
bool CSHDir::FindADir(string path,const string dirName)
{
    vector<string> result;
    vector<string>::iterator resultItr;
    
    /** 查找路径path下的所有目录，并将所有目录名放入result中 */
    SearchDir(path,result);
    
    /** 在结果中查找有无相同的目录名 */
    for(resultItr=result.begin();resultItr!=result.end();resultItr++)
    {
        /** 有该目录，则返回true */
        if (*resultItr==dirName) return true;
    }
    
    /** 无该目录，返回false */
    
    /** modify by wangwei at 20080123 */
    /// 到达该分支应当直接返回false
    ///if (resultItr==result.end())
    ///    return false;
    return false;
    /** modify by wangwei at 20080123  end */
}

#endif 





///#############################################################################
/// END
