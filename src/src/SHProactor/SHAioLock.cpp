#include "SHAioLock.h"
#include "ace/Token.h"

bool CSHAioLock::Lock()
{
    bool ret = true;
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    lock->acquire();
    return ret;
}
bool CSHAioLock::UnLock()
{
	bool ret = true;
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    lock->release();
	return ret;
}
CSHAioLock::CSHAioLock()
{
    m_lock = new ACE_Thread_Mutex();
}
CSHAioLock::~CSHAioLock()
{
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    delete lock;
}

