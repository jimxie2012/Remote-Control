#pragma once
#include "Common.h"

#define SH_MAX_BUF 1024

class SH_EXPORT CSHData 
{
public:
	CSHData(int maxSize );
	CSHData(void * data = NULL , int size = SH_MAX_BUF  );
	~CSHData();
public:
	void      Zero();
	bool      SetData(CSHData &data );
	bool      SetData(void * data, int len = SH_MAX_BUF);
	char *    GetData();
	int       GetDataSize();
	bool      SetDataSize(int size);
	bool      SetMaxSize(int size);
	int       GetMaxSize();
private:
	char      m_data[SH_MAX_BUF];   
	int       m_dataSize;
	int       m_maxSize;
};
