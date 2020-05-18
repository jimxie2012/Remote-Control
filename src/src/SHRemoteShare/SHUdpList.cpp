#include "SHUdpList.h"
#include<time.h>
#ifdef _LINUX
#include <unistd.h>
#include <stdlib.h>
#endif
#include "SHCommon.h"

CSHUdpList::CSHUdpList(){	
	Clear();
}
CSHUdpList::~CSHUdpList(){
}
void CSHUdpList::Clear(){
	m_lock.Lock();
	m_list.clear();
	m_lock.UnLock();
}
struct tm CSHUdpList::GetCurrentTime(){
	time_t tval;
	struct tm *now;
	tval= time(NULL);
	now = localtime(&tval);
	return *now;
}
bool CSHUdpList::Delete(SH_AGENT_ID id){
	bool ret = false;
	m_lock.Lock();
	map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= m_list.find(id );
	if ( it != m_list.end() )	{
		m_list.erase(it);
		ret = true;
	}
	m_lock.UnLock();
	return ret;
}
bool CSHUdpList::Add(SH_AGENT_ID id, CSHDataPulse::ST_DATA nodeData){
	m_lock.Lock();
	nodeData.m_activeTime = CSHUdpList::GetCurrentTime();
	map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= m_list.find(id );
	if ( it != m_list.end() )
            m_list.erase(it);
	m_list.insert(make_pair(id,nodeData));
	m_lock.UnLock();
	return true;
}
bool CSHUdpList::Find(SH_AGENT_ID id, CSHAddr &addr){
	bool ret = false;
	m_lock.Lock();
	map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= m_list.find(id );
	if ( it != m_list.end() ){
		addr.SetAddr(it->second.m_agentIp,it->second.m_agentPort);
		ret = true;
	}
	m_lock.UnLock();
	return ret;
}
bool CSHUdpList::IsActive(SH_AGENT_ID id,int maxSeconds){
      bool ret = false;
      m_lock.Lock();
      map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= m_list.find(id );
      if ( it != m_list.end() ){
           struct tm activeTime = it->second.m_activeTime;
           struct tm current = CSHCommon::GetCurrentDateTime();
           int seconds = CSHCommon::GetSeconds(current,activeTime);
           if ( seconds <= maxSeconds ) {
               ret = true;
           } 
      }
      m_lock.UnLock();
      return ret;
}
map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator CSHUdpList::FindNode(SH_AGENT_ID id){
	return m_list.find(id );
}
void  CSHUdpList::GetPulseList(map<SH_AGENT_ID,CSHDataPulse::ST_DATA> &OnLineList){
       OnLineList.clear();
       m_lock.Lock();
       map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator it= m_list.begin();
       for(it=m_list.begin();it!=m_list.end();++it){
           OnLineList.insert(make_pair(it->first,it->second));
        }
        m_lock.UnLock();
}
