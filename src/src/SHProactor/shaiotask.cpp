#include "SHAioTask.h"
#include "SHCommon.h"
#include "ace/Log_Msg.h"
#include <list>

CSHAioTask::CSHNode::CSHNode()
{ 
	 m_lock = new CSHAioLock();
}
CSHAioTask::CSHNode::~CSHNode()
{
	delete m_lock;
}
bool CSHAioTask::CSHNode::IsLoop()
{
    m_lock->Lock();
	bool ret = m_isLoop;
	m_lock->UnLock();
	return ret;
}
void CSHAioTask::CSHNode::SetLoop(bool flag)
{   
    m_lock->Lock();
	m_isLoop = flag;
	m_lock->UnLock();
}
CSHAioTask::CSHAioTask(void)
{
	m_list = new list<CSHNode*>;
	m_list->clear();
	m_proactorImpl = NULL;
}
CSHAioTask::~CSHAioTask(void)
{
	delete m_list;
}
void CSHAioTask::Init(int threadAccount)
{
#ifdef WIN32
        ACE_NEW (m_proactorImpl,ACE_WIN32_Proactor;(65535,0,0));
#else
        //fix a bug
        ACE_NEW (m_proactorImpl,ACE_Poll_Proactor(65535,0,0));
        //ACE_NEW (m_proactorImpl,ACE_LINUX_Proactor(65535,0,0));
#endif
        m_proactor = new ACE_Proactor( m_proactorImpl,1);
	for(int i=0;i< threadAccount;i++){
		CSHNode *node = new CSHNode();
		node->SetLoop( false );
                node->m_proactor = GetProactor();
		m_list->push_back(node);
	}
}
void CSHAioTask::Clear(){
	list<CSHNode *>::iterator it;
	for(it=m_list->begin();it!=m_list->end();++it){
		 CSHNode *tmp = *it;
		 delete tmp;
	}
	m_list->clear();
}
ACE_Proactor *CSHAioTask::GetProactor()
{
	return m_proactor;
}
SH_THREAD_RETURN CSHAioTask::LoopThread(void *pParam)
{
     CSHNode *node = (CSHNode*)pParam;
     node->SetLoop( true );
     node->m_proactor->proactor_run_event_loop(); 
     node->SetLoop( false );
     return NULL;
}
void CSHAioTask::BeginLoop()
{
    list<CSHNode*>::iterator it;
	for(it=m_list->begin();it!=m_list->end();++it)
	{
		 CSHAioTask::CSHNode *tmp = *it;
		 tmp->SetLoop(false);
		 while(!CSHCommon::BeginThread(LoopThread,tmp) ){
                     ACE_ERROR ((LM_ERROR,"BeginThread error retry\n"));
                     CSHCommon::Sleep(1);
                 }
		 while( !tmp->IsLoop() )
		 {
		      CSHCommon::Sleep(1);
		 }
	}
}
void CSHAioTask::EndLoop()
{
    list<CSHNode*>::iterator it;
	for(it=m_list->begin();it!=m_list->end();++it)
	{
		 CSHNode *tmp = *it;
		 if  ( tmp->IsLoop() )
		 {
		     tmp->m_proactor->proactor_end_event_loop();
		 }
	}
	while( true )
	{
	    bool findLoop=false;
	    for(it=m_list->begin();it!=m_list->end();++it)
	    {
		    CSHNode *tmp = *it;
		    if( tmp->IsLoop() )
		    {
		       findLoop = true;
		    }
	     }
		if(findLoop )
			CSHCommon::Sleep(1);
		else
			break;
	}
	Clear();
	if ( m_proactorImpl != NULL ){
       m_proactorImpl->close();
       delete m_proactorImpl;
	}
}
