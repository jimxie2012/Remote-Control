#pragma once
#include "Common.h"
#include "SHData.h"
#define   FILE_CMD_OK       "OK"
#define   FILE_CMD_ERROR    "ERROR"
#define   FILE_CMD_IO_ERROR "IO_ERROR"

class SH_EXPORT CSHFile 
{
public:
	CSHFile();
	~CSHFile();
public:
	bool        WriteLn(string str);
	string      ReadLn();
	bool        Open(string fileName, bool isRead = true);
	bool        Close();
	bool        Read(CSHData& data);
	bool        Write(CSHData data);
	bool        WriteString(string str);
	string      ReadString();
	static bool Remove(string fileName);
	bool        Seek(bool isEnd=false);
	static bool WriteFlag(string fileName,string flag);
	static string  ReadFlag(string fileName);
	static int  GetFileSize(string fileName);
private:
	FILE        *m_file;
};