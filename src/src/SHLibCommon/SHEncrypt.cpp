/// INCLUDES
#ifdef IBMAIX
#include <unistd.h>
#include <pcap.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#if defined(WIN32)
#include <WinSock2.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdexcept>
#include <strstream>
#include <sstream>
#include <set>
#include <iosfwd>

#include "SHEncrypt.h"
#include "SHString.h"
#include "CBase64.h"
#include "desBase64.h"
#include <fstream>

#define BUFFERSIZE 1024

using namespace std;

#ifndef MD
#define MD 5
#endif
#include "CBase64.h"
#include "desBase64.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#if MD == 2
#include "md2.h"
#endif
#if MD == 4
#include "md4.h"
#endif
#if MD == 5
#include "md5.h"
#endif

/* Length of test block, number of test blocks.
 */
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000

static void MDString PROTO_LIST((char *));
static void MDTimeTrial PROTO_LIST((void));
static void MDTestSuite PROTO_LIST((void));
static void MDFile PROTO_LIST((char *));
static void MDFilter PROTO_LIST((void));
static void MDPrint PROTO_LIST((unsigned char [16]));

#if MD == 2
#define MD5_CTX MD2_CTX
#define MDInit MD2Init
#define MDUpdate MD2Update
#define MDFinal MD2Final
#endif
#if MD == 4
#define MD5_CTX MD4_CTX
#define MDInit MD4Init
#define MDUpdate MD4Update
#define MDFinal MD4Final
#endif
#if MD == 5
#define MD5_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final
#endif

///#############################################################################
/// CLASS Name  : CEncryptDecrypt
////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERFACE MEMBER FUNCTIONS
int32 CSHEncrypt::EncryptDecryptData(int32 type, const char* pInput, int32 lenInput, char *pOutput, 
										  int32 *lenOutput, int32 cryType, const char* pKeyCh)
{

    /// 用来保存中间结果
    char desDest[1024];
    memset(desDest, 0, 1024);

    /// 加密
    if(type==0)
    {
		
         if(DesEncrypt(pKeyCh, pInput, pOutput, desDest)<0)
         {
             return -1;
         }
	
    }
    /// 解密
    else if(type ==1)
    {
        if(DesDecrypt(pKeyCh, pInput, pOutput, desDest)<0)
        {
            return -1;
        }

    }
    /// 参数错误
    else
    {
        return -1;
    }
    *lenOutput = (int)strlen(pOutput);
    return 0;
}


int32 CSHEncrypt::TripleDesFixKeyEncryptDecryptData(int32 type, const char* pInput, char *pOutput, int32 cryType)
{


    if(type == 1)
    {
        /// 调用加解密函数
        if(DesEncryptFixKey(pInput, pOutput )< 0)  
            return -1;			
	    
    }
    else if(type == 0)
    {
        if(DesDecryptFixKey(pInput, pOutput )<=  0)
            return -1;
    }
    else
    {
        return -1;
    }

    return 0;	
}

int32 CSHEncrypt::DigestInfo(const char* pOriginalText, int32 lenOriginalText, 
										char *pDigestInfo, int32 *lenDigest,int32 digestArithType)
{
    if((pDigestInfo == NULL)||(pOriginalText == NULL))
        return -1;
        
    if(strlen(pOriginalText) != (size_t)lenOriginalText)
        return -1;
	 
    /// 生成摘要
    MD5_CTX context;
    unsigned char digest[16];
    unsigned int len = (unsigned int)strlen(pOriginalText);
    MDInit (&context);
    MDUpdate (&context, (unsigned char*)pOriginalText, len);
    MDFinal (digest, &context);
  
    /// 对摘要编码
    unsigned int iOutSize;
    Base64 base;
    char *pTempCh = (char *)base.operate_buf( int('e'), digest, 16, iOutSize) ;
	
    strncpy(pDigestInfo, pTempCh, strlen(pTempCh));
    *lenDigest = (int)strlen(pTempCh);
    delete []pTempCh;
    return 0;
}
string CSHEncrypt::Encrypt(string minwen){
	char source[4096] = { 0 };
	strcpy(source,minwen.c_str());
	char dest[4096]= { 0 };
	DesEncryptFixKey(source,dest);
	return dest;
}
string CSHEncrypt::Decrypt(string miwen){
	char source[4096] = { 0 };
	strcpy(source,miwen.c_str());
	char dest[4096]= { 0 };
	DesDecryptFixKey(source,dest);
	return dest;
}
///#############################################################################
/// END

