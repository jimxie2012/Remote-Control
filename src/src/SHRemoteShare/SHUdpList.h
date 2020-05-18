#pragma once
#include <map>
#include <list>
#include "SHSessionData.h"
#include "SHLock.h"
#include "SHAddr.h"
#include "SHAio.h"

class CSHUdpList  
{
public:	
	CSHUdpList();
	~CSHUdpList();
public:
        void                               GetPulseList(map<SH_AGENT_ID,CSHDataPulse::ST_DATA> &OnLineList);
	bool                               Find(SH_AGENT_ID id,CSHAddr &addr);
        bool                               IsActive(SH_AGENT_ID id,int maxSeconds);
	void                               Clear();
	bool                               Delete(SH_AGENT_ID id);
	bool                               Add(SH_AGENT_ID id,CSHDataPulse::ST_DATA nodeData);
private:
	struct tm                          GetCurrentTime();
	map<SH_AGENT_ID,CSHDataPulse::ST_DATA>::iterator  FindNode(SH_AGENT_ID id);
private:
        CSHLock                            m_lock;
	map<SH_AGENT_ID,CSHDataPulse::ST_DATA>            m_list;
};
