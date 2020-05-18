#pragma once
#pragma once
#include "Common.h"
#include "SHLock.h"
#include "SHAddr.h"
#include "SHData.h"

#define   SOCKET_TIME_OUT      30

class CSHData;

class SH_EXPORT CSHSocket
{  
public:
	/// Defines all error codes handled by the CSimpleSocket class.
   typedef enum 
    {
        SocketError = -1,          ///< Generic socket error translates to error below.
        SocketSuccess = 0,         ///< No socket error.
        SocketInvalidSocket,       ///< Invalid socket handle.
        SocketInvalidAddress,      ///< Invalid destination address specified.
        SocketInvalidPort,         ///< Invalid destination port specified.
        SocketConnectionRefused,   ///< No server is listening at remote address.
        SocketTimedout,            ///< Timed out while attempting operation.
        SocketEwouldblock,         ///< Operation would block if socket were blocking.
        SocketNotconnected,        ///< Currently not connected.
        SocketEinprogress,         ///< Socket is non-blocking and the connection cannot be completed immediately
        SocketInterrupted,         ///< Call was interrupted by a signal that was caught before a valid connection arrived.
        SocketConnectionAborted,   ///< The connection has been aborted.
        SocketProtocolError,       ///< Invalid protocol for operation.
        SocketFirewallError,       ///< Firewall rules forbid connection.
        SocketInvalidSocketBuffer, ///< The receive buffer point outside the process's address space.
        SocketConnectionReset,     ///< Connection was forcibly closed by the remote host.
        SocketAddressInUse,        ///< Address already in use.
        SocketInvalidPointer,      ///< Pointer type supplied as argument is invalid.
        SocketEunknown             ///< Unknown error please report to mark@carrierlabs.com
    } CSocketError;   
    typedef enum 
    {
	    STATUS_ERROR      = -1,   //Select 错误
        STATUS_TIME_OUT   = 0,    //Select 超时
	    STATUS_CAN_READ   = 1,    //Select 可读
		STATUS_CAN_WRITE  = 2,    //Select 可写
		STATUS_CAN_BOTH   = 3     //Select 可读可写
    } CSocketStatus;
public:
	CSHSocket();
	~CSHSocket();
public:
	//socket base action
    CSocketError    GetSocketError(void);
	bool            Create(bool isTcp = true);
	bool            Bind(CSHAddr& addr);
	bool            Read(CSHData& data,int timeOut =SOCKET_TIME_OUT );
	bool            ReadLine(string &line);
	bool            Write(CSHData data);
	void            Close();
	bool            Listen(int maxClient = 0);
	bool            Accept(CSHSocket& sClient, CSHAddr* sClientAddr = NULL);
	bool            Connect(CSHAddr & addr);
	int             ReadCmd();
	bool            SendCmd(int cmd);
	bool            SendFile(string fileName);
	bool            ReadFile(string fileName);
public:
	//socket extern action
	CSHAddr&        GetPeerAddr();
	CSHAddr&        GetLocalAddr();
	int             GetSocket();
	void            SetSocket(int  s);
	static    int   GetFreePort();
private:
	CSocketStatus   Select(int nTimeoutSec, int nTimeoutUSec);
	void            TranslateSocketError(void);
	void            SetSocketError(CSHSocket::CSocketError error);
public:
	//socket extern action
	int             TcpSend(const char * buf, int len);
	int             TcpRead(char *buf,int len);
private:
	int             m_socket;
	bool            m_isTcp;
	CSHAddr         m_remoteAddr;
	CSHAddr         m_localAddr;
    CSocketError    m_socketErrno;       /// number of last error
};
