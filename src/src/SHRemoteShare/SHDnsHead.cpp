#include "SHDnsHead.h"
#include <string.h>
#include "SHString.h"
#include "SHCommon.h"
#include "SHSessionData.h"
#include "stdio.h"
#ifdef WIN32
#include <winsock.h>
#endif

CSHDnsHead::CSHDnsHead(void){
}
CSHDnsHead::~CSHDnsHead(void){
}
char * CSHDnsHead::DeRequest(char *buf,int size){
   char ret[1024] = {'\0'};
   if ( size > ( sizeof(dns_header) + 1 ) )
       strcpy(ret,buf + sizeof(dns_header) + 1);
   return ret;
}
void CSHDnsHead::EnRequest(char *domain){
        char    *p;
        m_size = 0;
        memset(m_buf,0,sizeof(m_buf));
        dns_header  *dnshdr = (dns_header *)m_buf;
        dns_query   *dnsqer;
        dnshdr->id = (u16)1;  
        dnshdr->flags = htons(0x0100);  
        dnshdr->nques = htons(1);  
        dnshdr->nanswer = htons(0);  
	    dnshdr->nauth = htons(0);  
	    dnshdr->naddi = htons(0);  
	    strcpy(m_buf + sizeof(dns_header) + 1, domain);  
	    p = m_buf + sizeof(dns_header) + 1;
	    int i =0;
	    while (p < (m_buf + sizeof(dns_header) + 1 + strlen(domain))) {  
	        if ( *p == '.'){  
	           *(p - i - 1) = i;  
	           i = 0;  
	        } else{  
	           i++;  
	        }  
	        p++;  
	    }  
	    *(p - i - 1) = i;  
	    dnsqer = (dns_query *)(m_buf + sizeof(dns_header) + 2 + strlen(domain));  
	    dnsqer->dns_class = htons(1);  
	    dnsqer->type = htons(1);  
	    m_size = sizeof(dns_header) + sizeof(dns_query) + strlen(domain) + 2;   
	 }
