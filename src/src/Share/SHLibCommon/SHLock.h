#pragma once
#include "Common.h"

class SH_EXPORT CSHLock 
{
public:
	CSHLock();
	~CSHLock();
public:
	bool Lock();
	bool UnLock();
private:
    void *m_lock;
};
