#include <SHLock.h>
#include "ace/Token.h"

bool CSHLock::Lock()
{
	bool ret = true;
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    lock->acquire();
	return ret;
}
bool CSHLock::UnLock()
{
	bool ret = true;
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    lock->release();
	return ret;
}
CSHLock::CSHLock()
{
    m_lock = new ACE_Thread_Mutex();
}
CSHLock::~CSHLock()
{
    ACE_Thread_Mutex *lock = ( ACE_Thread_Mutex *)m_lock;
    delete lock;
}

