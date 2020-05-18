#pragma once
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/Asynch_IO_Impl.h"
#include "ace/POSIX_Proactor.h"
#ifdef WIN32
#include "ace/WIN32_Proactor.h"
#endif
#include "SHAioLock.h"
#include "SHCommon.h"
#include <list>
using namespace std;

class CSHAioTask 
{
private:
   class CSHNode{
        friend class CSHAioTask;
   public:
	CSHNode();
	~CSHNode();
   public:
	bool IsLoop();
        void SetLoop(bool flag);
   private:
	    CSHAioLock             *m_lock;
	    bool                    m_isLoop;
        ACE_Proactor           *m_proactor;
   };
public:
	CSHAioTask(void);
	~CSHAioTask(void);
public:
	ACE_Proactor *GetProactor();
	void Init(int threadAccount);
	void BeginLoop();
	void EndLoop();
private:
	static SH_THREAD_RETURN LoopThread(void *pParam);
private:
	void     Clear();
private:
	CSHAioLock                           m_lock;
	list< CSHAioTask::CSHNode* >        *m_list;
        ACE_Proactor                        *m_proactor;
#ifdef WIN32
        ACE_WIN32_Proactor                  *m_proactorImpl;
#else
        ACE_POSIX_Proactor                  *m_proactorImpl;
#endif
};

