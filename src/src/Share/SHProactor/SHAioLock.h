#pragma once
#if defined (WIN32)
#define SH_AIO_EXPORT __declspec(dllexport)
#else 
#define SH_AIO_EXPORT 
#endif

class SH_AIO_EXPORT CSHAioLock 
{
public:
	CSHAioLock();
	~CSHAioLock();
public:
	bool Lock();
	bool UnLock();
private:
    void *m_lock;
};
