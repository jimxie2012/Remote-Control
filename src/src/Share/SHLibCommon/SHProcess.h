#pragma once
#include "Common.h"

class SH_EXPORT CSHProcess
{
public:
    static bool     KillProgress(int pid);
    static int      OpenProgress(const char *cmdLine);
};
