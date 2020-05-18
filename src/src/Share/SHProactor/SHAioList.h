#pragma once
#include "SHAioLock.h"
#include <iostream>
#include <list>
#include <map>
//#include "SHAio.h"
using namespace std;

class CSHAio;

class SH_AIO_EXPORT CSHAioList
{
public:
    CSHAioList(void);
    ~CSHAioList(void);
public:
    CSHAio*         Alloc(int checkTimer,int timeOut);
    int             GetBusyAccount();
    void            CloseAll();
    void            Add(CSHAio *io);
    list<CSHAio *> *GetList();
private:
    CSHAioLock     m_lock;
    list<CSHAio *> *m_list;
};
