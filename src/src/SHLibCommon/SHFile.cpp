#include <SHFile.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>;
#include <sys/stat.h>;
CSHFile::CSHFile()
{
	m_file = NULL;
}
CSHFile::~CSHFile()
{
}
bool CSHFile::Open(string fileName, bool isRead)
{
	if ( isRead )
	{
		m_file = fopen(fileName.c_str(), "ab+" );
		if( m_file == NULL ) 
		{
			return false;
		}
	}
	else
	{	
		m_file = fopen(fileName.c_str(), "a+b" );	
		if( m_file == NULL ) 
		{
			return false;
		}
	}
	return Seek(false);
}
bool CSHFile::Close()
{
   if ( m_file != NULL )
	   fclose(m_file);
   m_file = NULL;
   return true;
}
bool CSHFile::Read(CSHData& data)
{
	bool ret = false;
	int size = fread(data.GetData(),1,data.GetMaxSize(),m_file);
	if ( size > 0 )
	{
		data.SetDataSize( size );
		ret = true;
	}
	return ret;
}
bool CSHFile::Write(CSHData data)
{
   if ( fwrite(data.GetData(),1,data.GetDataSize(),m_file) != data.GetDataSize() )
		return false;
   return true;
}
bool CSHFile::Seek(bool isEnd)
{
    if ( !isEnd )
	{
		if ( fseek(m_file,0,SEEK_SET) != 0)
		{
			return false;
		}
	}
	else
	{	
        if ( fseek(m_file,0,SEEK_END) != 0)
		{
			return false;
		}
	}
	return true;
}
bool CSHFile::WriteString(string str)
{
	CSHData data;
	int len = strlen(str.c_str());
	data.SetData(&len,sizeof(len));
	bool ret = Write(data);
	if ( ret )
	{
		data.SetData((void *)str.c_str(),len);
        ret = Write(data);
	}
	return ret;
}
string CSHFile::ReadString()
{
    string ret = "";
    int len = 0 ;
	CSHData data;
	data.SetMaxSize(sizeof(len));
	if ( Read(data) )
	{
		memcpy(&len,data.GetData(),sizeof(len));
		if (( len > 0 ) && (len < 65535))
		{
			data.Zero();
			if ( Read(data) )
			{
				ret = data.GetData();
			}
		}
	}
	return ret;
}
bool CSHFile::Remove(string fileName)
{
	if ( ::remove(fileName.c_str()) ==0 )
		return true;
	return false;
}

string CSHFile::ReadLn()
{
	string ret = ""; 
    char line [1024]= { 0 };
    if( fgets( line,sizeof(line),m_file ) != NULL)
	{
		for ( int i = 0 ; i < strlen(line); i ++ )
		{
			if ( line[i] == '\n' )
			{
				line[i] = '\0';
			}
		}
		ret = line;
	}
	return ret;
}

bool CSHFile::WriteLn(string str)
{
	bool ret = false; 

    if( fputs( str.c_str(),m_file ) >= 0)
		ret = true;
	return ret;
}
bool CSHFile::WriteFlag(string fileName,string flag)
{
	bool ret = false;
    CSHFile file;
	if ( file.Open(fileName,false) )
	{
		if ( file.Seek(false) )
		{
			ret = file.WriteLn(flag);
		}
		file.Close();
	}
	return ret;
}
string  CSHFile::ReadFlag(string fileName)
{
	string ret = FILE_CMD_IO_ERROR;
    CSHFile file;
	if ( file.Open(fileName,true) )
	{
		if ( file.Seek(false) )
		{
			string tmp = file.ReadLn();
			if ( tmp != "" )
				ret = tmp;
		}
		file.Close();
	}
	return ret;
}
int  CSHFile::GetFileSize(string fileName){
	struct stat     f_stat;
	if (stat(fileName.c_str(), &f_stat) == -1) {
        return -1;
    }
    return (int)f_stat.st_size;
}