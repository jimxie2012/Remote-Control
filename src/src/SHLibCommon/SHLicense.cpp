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
#include "SHLicense.h"
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
/// INLINED FUNCTIONS

///#############################################################################
///  CLASS Name  : CLicenseTools
/**
  * 对license内容进行加密形成key
  * @param content[in]: license文件的具体内容
  * @param size[in]: license内容字符串的大小
  * @param bufferKey[out]: 保存licenseKey的字符串
  * @param bufOfLen[out]: licenseKey的大小
  */  
int CSHLicense::CreateLicenseKey( const char*content, const int size, char *bufferKey, int &bufOfLen)
{ 
    char MD5Buffer[BUFFERSIZE];
    int md5buflen;
    memset( MD5Buffer, 0, BUFFERSIZE);
   
    CSHEncrypt::DigestInfo( content, size, MD5Buffer, &md5buflen);
    CSHEncrypt::TripleDesFixKeyEncryptDecryptData( 1, MD5Buffer, bufferKey);
    bufOfLen = (int)strlen( bufferKey);
    ///CEncryptDecrypt::EncryptDecryptData( 0, MD5Buffer, md5buflen, bufferKey, &bufOfLen);
    return 0;
}


/**
  * 验证license是否合法
  * @param content[in]: license的内容
  * @param size[in]: license的长度
  * @param key[in]: key的内容
  * @param keysize[in]: key的长度
  * @return: 0表示验证成功,-1表示验证失败
  */
int CSHLicense::CheckLicense( const char*content, const int size, const char*key, const int keysize)
{
    char tempForLicense[BUFFERSIZE];
    memset( tempForLicense, 0, BUFFERSIZE);
    int32 tempLicenseLen;

    char tempForKey[BUFFERSIZE];
    memset( tempForKey, 0, BUFFERSIZE);
    //int32 tempKeySize;

    CSHEncrypt::DigestInfo( content, size, tempForLicense, &tempLicenseLen);
    ///CEncryptDecrypt::EncryptDecryptData( 1, key, keysize, tempForKey, &tempKeySize);
    CSHEncrypt::TripleDesFixKeyEncryptDecryptData( 0, key, tempForKey);
    if( strcmp( tempForLicense, tempForKey) ==0)
        return 0;
    else return -1;
}

bool CSHLicense::CheckLicenseByFile( char* licenseFile, char*keyFile)
{
    ifstream strmLicense;
    strmLicense.open( licenseFile, ios::binary);
    
    ifstream strmKey;
    strmKey.open( keyFile);

    string strLicenseContent;
    getline( strmLicense, strLicenseContent, (char) -1);
  ///  cout << strLicenseContent.c_str() << endl;
    
    string strKeyContent;
    getline( strmKey, strKeyContent, (char) -1); 
 ///   cout << strKeyContent.c_str() << endl;
    
    strmLicense.close();
    strmKey.close();
    
    int index = 0;
    while( (index = (int)strLicenseContent.find("\r")) !=-1)
    {
        strLicenseContent.replace( index, 1, "");
    }
    if( CheckLicense( strLicenseContent.c_str(),(const int) strLicenseContent.length(), strKeyContent.c_str(), (const int)strKeyContent.length()) == 0)
        return true;
    else return false;
    
}


///#############################################################################
/// END

