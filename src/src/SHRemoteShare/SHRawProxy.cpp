#include <stdio.h>
#include <SHSessionData.h>
#include "SHRawProxy.h"
#include "SHCommon.h"
#include <stdlib.h>
CSHRawProxy::CSHRawProxy(){
     m_isWorking = false;
}
CSHRawProxy::~CSHRawProxy(){
}
SH_THREAD_RETURN CSHRawProxy::UdpThread1(void *param){
     CSHRawProxy *proxy = ( CSHRawProxy * )param;
     while( proxy->m_isWorking ){
          CSHDataPulse data;
          if ( proxy->m_udp1.ReadPulse ( data ) ){
	  printf("pulse msg from %d id=%d,acl=%s,privateIp=%s,admin_user=%s,publicIp=%s\n",proxy->m_udp1.m_localAddr.GetPort(),data.GetId(),data.GetAcl(),data.GetPrivateIp(),data.GetAdminUser(),proxy->m_udp1.GetPeer().GetIpString().c_str());
          }
    }
    return 0;
}
SH_THREAD_RETURN CSHRawProxy::UdpThread2(void *param){
     CSHRawProxy *proxy = ( CSHRawProxy * )param;
     while( proxy->m_isWorking ){
          CSHDataPulse data;
          if ( proxy->m_udp2.ReadPulse ( data ) ){
          printf("pulse msg from %d id=%d,acl=%s,privateIp=%s,admin_user=%s,publicIp=%s\n",proxy->m_udp2.m_localAddr.GetPort(),data.GetId(),data.GetAcl(),data.GetPrivateIp(),data.GetAdminUser(),proxy->m_udp2.GetPeer().GetIpString().c_str());
          }
    }
    return 0;
}
SH_THREAD_RETURN CSHRawProxy::UdpThread3(void *param){
     CSHRawProxy *proxy = ( CSHRawProxy * )param;
     while( proxy->m_isWorking ){
          CSHDataPulse data;
          if ( proxy->m_udp3.ReadPulse ( data ) ){
          printf("pulse msg from %d id=%d,acl=%s,privateIp=%s,admin_user=%s,publicIp=%s\n",proxy->m_udp3.m_localAddr.GetPort(),data.GetId(),data.GetAcl(),data.GetPrivateIp(),data.GetAdminUser(),proxy->m_udp3.GetPeer().GetIpString().c_str());
          }
    }
    return 0;
}
void CSHRawProxy::Stop(){
	m_isWorking = false;
	m_udp1.Close();
        m_udp2.Close();
	this->StopServer();
}
bool CSHRawProxy::Start(int port,
			int udpPort1,
                        int udpPort2,
                        int udpPort3,
			int maxIO,
			int checkTimer,
			int timeOut,
                        int sleepTime,
			int threadAccount){
	if ( !m_udp1.Create() ){
	   printf("creat udp proxy error %d\n",port);
	   return false;
	}
        m_udp1.SetSleepTime(sleepTime);
	if ( !m_udp1.Bind(udpPort1,"0.0.0.0") ){
	   printf("bind udp proxy error %d\n",udpPort1);
	   return false;
	}
        if ( !m_udp2.Create() ){
           printf("creat udp proxy error %d\n",udpPort2);
           return false;
        }
        m_udp2.SetSleepTime(sleepTime);
        if ( !m_udp2.Bind(udpPort2,"0.0.0.0") ){
           printf("bind udp proxy error %d\n",udpPort2);
           return false;
        }
        if ( !m_udp3.Create() ){
           printf("creat udp proxy error %d\n",udpPort3);
           return false;
        }
        m_udp3.SetSleepTime(sleepTime);
        if ( !m_udp3.Bind(udpPort3,"0.0.0.0") ){
           printf("bind udp proxy error %d\n",udpPort3);
           return false;
        }
        m_isWorking = true;
	if ( !CSHCommon::BeginThread(UdpThread1,this)){
	   printf("start udp thread error\n");
	   return false;
	}
        if ( !CSHCommon::BeginThread(UdpThread2,this)){
           printf("start udp thread error\n");
           return false;
        }
        if ( !CSHCommon::BeginThread(UdpThread3,this)){
           printf("start udp thread error\n");
           return false;
        }
          
  	return StartServer(port,maxIO,checkTimer,timeOut,threadAccount);
}
void CSHRawProxy::OnAccept(SH_HANDLE s){
     SetPeer(s,0);
     Read(s,CSHDataHead::GetSize());
}
void CSHRawProxy::OnConnect(SH_HANDLE s){
}
void CSHRawProxy::OnMessage(SH_HANDLE s,const char *buf,int size){
    SH_HANDLE peer = this->GetPeer(s);
    if ( peer == 0 ){
        if ( size == CSHDataHead::GetSize() ){
            CSHDataHead data(buf);
            if ( data.GetCmd() == CMD_CONNECT ){
		 SH_HANDLE session = data.GetSession();
		 this->SetPeer(s,session);
		 data.SetSession(s);
                 /*
		 int key = rand()%32766;
		 while ( key == 0 )
		     key = rand()%32766;
		 data.SetKey(key);
		 for ( int ttt = 0 ; ttt < 3 ; ttt ++ ){
		     if ( !m_udp.SendConnect(session,data) ){
		        data.SetFlag( false );
			this->Write(s,(char *)data.GetData(),CSHDataHead::GetSize());
			this->Close(s);
		     }
                     break;
	             CSHCommon::Sleep(1);
		}
                */
                data.SetKey(0);
                bool flag = m_udp1.SendConnect(session,data);
                if (!flag) 
                    flag = m_udp2.SendConnect(session,data);
                if (!flag) 
                    flag = m_udp3.SendConnect(session,data);
                if ( !flag ){
                   printf("can not send connect info to %u\n",session);
                   data.SetFlag( false );
                   this->Write(s,(char *)data.GetData(),CSHDataHead::GetSize());
                   this->Close(s);
                }
                return;
            }
            if ( data.GetCmd() == CMD_CONNECT_ACK ){
               SH_HANDLE peerPeer = this->GetPeer(peer);
                if ( data.GetFlag() ){
                   SH_HANDLE session = data.GetSession();
                   this->SetPeer(s,session);
                   this->SetPeer(session,s);
		   if( ( !this->Read(s,0) ) || ( !this->Read(session,0) ) )
                      data.SetFlag(false);
    		   data.SetSession(peerPeer);
                   this->Write(session,(char *)data.GetData(),CSHDataHead::GetSize());
                }
                else{
		   data.SetFlag(false);
                   this->Write(peer,(char *)data.GetData(),CSHDataHead::GetSize());
                   this->Close(s);
                }
                return;
            }
            if ( data.GetCmd() == CMD_CLOSE ){
                this->Close(s);
                return;
            }
            printf("error cmd %d\n",data.GetCmd());
        }
        else{
            printf("size error \r\n");
        }
     }
     else{
        if( Write(peer,buf,size) )
            Read(s,0);
     }
}
void CSHRawProxy::OnClose(SH_HANDLE s){
     CSHProactor::OnClose(s);
}
