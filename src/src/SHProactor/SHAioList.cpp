#include "SHAioList.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/Asynch_IO_Impl.h"
#include "AceAio.h"
#include "SHCommon.h"

CSHAioList::CSHAioList(void)
{
	 m_list = new list<CSHAio *>;
}
CSHAioList::~CSHAioList(void)
{
	m_list->clear();
	delete m_list;
}
void CSHAioList::CloseAll()
{
    m_lock.Lock();
    list<CSHAio *>::iterator it;
    for(it=m_list->begin();it!=m_list->end();++it){
        if( (*it)->IsBusy() ){
            (*it)->Close();
        }
    }
    m_lock.UnLock();
}
int CSHAioList::GetBusyAccount()
{
    int account = 0;
    m_lock.Lock();
    list<CSHAio *>::iterator it;
    for(it=m_list->begin();it!=m_list->end();++it){
        if( (*it)->IsBusy() ){
            account++;
        }
    }
    m_lock.UnLock();
	return account;
}
CSHAio* CSHAioList::Alloc(int checkTimer,int timeOut)
{  
    CSHAio* ret = NULL;
    m_lock.Lock();
    list<CSHAio*>::iterator it;
    for(it=m_list->begin();it!=m_list->end();++it){
       CSHAio *tmp = *it;
       tmp->Lock();
       if( !tmp->m_isBusy ){
          ret = tmp;
          ret->m_activeTime = CSHCommon::GetCurrentDateTime();
          ret->m_timeOut    = timeOut;
          ret->m_isBusy     = true;
		  ret->m_checkTimer = checkTimer;
          tmp->Close(); ///add by jim 2018-03-08
          tmp->UnLock();

          break;
       }
       tmp->m_lock.UnLock();
    }
    m_lock.UnLock();
    return ret;
}
void CSHAioList::Add(CSHAio *io){
    m_lock.Lock();
    m_list->push_back(io);
    m_lock.UnLock();
}
list<CSHAio *> *CSHAioList::GetList(){
    return m_list;
}
