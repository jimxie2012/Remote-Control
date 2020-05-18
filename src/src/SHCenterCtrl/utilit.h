#include "SHAio.h"
#include "SHSessionData.h"
#define KIND_PROXY    "proxy"
#define KIND_SERVER   "server"
#define KIND_CLIENT   "client"
#define KIND_ALLOC    "alloc"
extern "C"{
void SH_ProactorInit(bool enableLog,unsigned int logLevel);

int SH_AllocPort(const char *vServerIp,
                 int vServerUdpPort,
                 SH_HANDLE agentId,
                 const char *remoteHost,
                 int remotePort,
                 int lockTime,
                 const char *userName,
                 const char *passwd);
bool SH_StartProxy(int dataPort,
                   int pulsePort1,
                   int pulsePort2,
                   int pulsePort3,
                   int maxAio,
                   int checkTimer,
                   int timeOut,
                   int sleepTime,
                   int threadAccount);
bool SH_StartClient(int maxAio,
                    int checkTimer,
                    int timeOut,
                    int threadAccount,
                    SH_HANDLE agentId,
                    const char *agentPass,
                    const char *proxyHost,
                    int   proxyPulsePort1,
                    int   proxyPulsePort2,
                    int   proxyPulsePort3,
                    int   sleepTime);
bool SH_StartServer(int maxAio,
                    int checkTimer,
                    int timeOut,
                    int threadAccount,
                    SH_HANDLE agentId,
                    const char *agentPass,
                    const char *proxyHost,
                    int   proxyDataPort,
                    int   udpPort,
                    int   tcpPortList[],
                    int   tcpPortAccount,
                    int   maxLockTime);
int SH_BeginLoop();
int SH_RunTimeLoop();
int SH_StopClient();
int SH_GetOnLine(map<SH_AGENT_ID,CSHDataPulse::ST_DATA> &OnLineList);
}
