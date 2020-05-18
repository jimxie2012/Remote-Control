#pragma once
typedef unsigned short u16;  
typedef struct{     
    u16 id;  
    u16 flags;  
    u16 nques;  
    u16 nanswer;  
    u16 nauth;  
    u16 naddi; 
}dns_header;
typedef struct{
    u16 type;
    u16 dns_class;
}dns_query;

class CSHDnsHead
{
public:
	CSHDnsHead(void);
	~CSHDnsHead(void);
public:
	char *DeRequest(char *buf,int size);
    void EnRequest(char *domain);	
public:
    char m_buf[1024];
    int  m_size;
};

