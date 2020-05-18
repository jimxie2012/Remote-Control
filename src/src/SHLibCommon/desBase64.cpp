#if defined(WIN32)
#include <io.h>
#endif
#include <stdio.h>
#include <string.h>
#include "CBase64.h"
#include "desBase64.h"
#include "d3des.h"


#define BUFFERSIZE 1024
static void patch(char* input, char*key, int iInput, char*output, int &iOutput)
{

    //char *temp = new char[iInput+8];
    int nInput = iInput/2;
    int i =0;
    for ( i=0; i<3; i++)
    {
        output[i] = key[i];
    }
    for ( i=0; i<nInput; i++)
    {
        output[i+3] = input[i];
    }
    for ( i=0; i<2; i++)
    {
        output[i+3+nInput] = key[i+3];
    }
    for ( i=0; i< iInput-nInput; i++)
    {
        output[i+5+nInput] = input[nInput+i];
    }
    for (i=0; i<3; i++)
    {
        output[i+5+iInput] = key[i+5];
    }
    iOutput = iInput+8;
    //return temp;
}
static int detach( char*input, char*key, int iInput, char*output, int& iOutput)
{
    //output = new byte[iInput-8];
    if ( iInput < 8)
    {
        return 0;
    }
    int pos1 = 3;
    int pos2 = 3+(iInput-8)/2;
    int pos3 = 5+(iInput-8)/2;
    int pos4 = 5+iInput-8;
    int pos5 = iInput;
    int i = 0;
    for ( i=0; i<pos1; i++)
    {
        key[i] = input[i];
    }
    for ( i=0; i<pos2-pos1; i++)
    {
        output[i] = input[pos1+i];
    }
    for ( i=0; i< pos3-pos2; i++)
    {
        key[i+pos1] = input[pos2+i];
    }
    for ( i=0; i< pos4-pos3; i++)
    {
        output[pos2-pos1+i] = input[pos3+i];
    }
    for ( i=0; i< pos5-pos4; i++)
    {
        key[5+i] = input[pos4+i];
    }
    iOutput = iInput -8;
    return 1;
}

int DesEncrypt( const char *pKey, const char* source, char*dest, char*desDest)//source 原文 dest加密后的数据
{
    int len;
    int nSource = (int)strlen( source);
    nSource =  nSource -nSource%8 + 8;
    int n = nSource/8;
    unsigned char key[8];

    memset(key, 0, 8);
    if(strlen(pKey) != 8)
    {
        return -1;
    }
    memcpy(key, pKey, strlen(pKey));

    deskey( key, EN0);
    char *tempSource = new char[ nSource+1];
    int i;
    for ( i=0; i< n; i++)
    {
        char temp1[8];
        char temp2[8];
        for ( int j=0; j<8; j++)
        {
            temp1[j] = 0x0;
            temp2[j] = 0x0;
        }
        strncpy( temp1,&source[8*i], 8);
        
        des( (unsigned char*)temp1, (unsigned char*)temp2);
        strncpy( &tempSource[8*i], temp2, 8);
        
    }
    
    ///拷贝中间结果到临时变量中
    strncpy( desDest, tempSource, 8*i);
    desDest[8*i] = 0x0;
   
    /// 对中间结果进行编码并拷贝到密文空间
    Base64 base;
    unsigned int nOutSize;
    unsigned char *ptemp = base.operate_buf( (int)'e', (unsigned char*)desDest,(unsigned int) nSource, nOutSize);
    len = ((nSource+7)/3)*4 + 6;
    char *ppp = (char*)ptemp;

    strncpy( dest, (char *)ptemp, strlen(ppp));
    dest[len] = 0x0;
   
    /// 回收分配的临时空间
    delete []tempSource;
    delete []ptemp;
    return len;

}

int DesDecrypt( const char *pKey, const char* source, char*dest, char*desDest)// source 加密后的数据 dest解密后的数据
{
    int nSource = (int)strlen( source);
    /// 对密文解码
    Base64 base;
    unsigned int nOutSize =0;
    unsigned char *ptemp =base.operate_buf( 'd', (unsigned char*)source, nSource, nOutSize);
    nSource = ((nSource-6)/4)*3 + 3;
    nSource =  nSource -nSource%8 + 8;
    int n = (nSource-8)/8;
    
    unsigned char key[8];
    memset(key, 8, 0);
    if(strlen(pKey) !=8)
    {
        return -1;
    }
    memcpy(key, pKey, strlen(pKey));
    deskey( key, DE1);
    char *tempSource = new char[ nSource];
    memset(tempSource, 0, nSource);
    int i;
    for ( i=0; i< n; i++)
    {
        char temp1[8];
        char temp2[8];
        for ( int j=0; j<8; j++)
        {
            temp1[j] = 0x0;
            temp2[j] = 0x0;
        }
        strncpy( temp1,(char*)&ptemp[8*i], 8);
        des( (unsigned char*)temp1, (unsigned char*)temp2);
        strncpy( &tempSource[8*i], temp2, 8);
    }
    /// 拷贝明文到接收空间
    if(i>0)
    {
        int tempLen = (int)strlen(tempSource);
        strncpy ( dest, tempSource, tempLen);
        delete []tempSource;
        delete [] ptemp;
        return tempLen;
    }
    delete []tempSource;
    delete [] ptemp;
    return -1;
}

int DesEncryptFixKey(const char* source, char*dest)
{
    char desDest[1024];
    memset(desDest, 0, 1024);
    
    int len = 0;
    int nSource = (int)strlen( source);
    nSource =  nSource -nSource%8 + 8;
    int n = nSource/8;

    unsigned char pKey[8] = { 0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
    deskey( pKey, EN0); ///initialize key and action(EN0->encryt DE1->decrypt) 
    char *tempSource = new char[ nSource+1];
    int i;
    for ( i=0; i< n; i++)
    {
        char temp1[8]; 
        char temp2[8];
        memset( temp1, 0, 8);               
        memset( temp2, 0, 8); ///initialize temp1 and temp2
        strncpy( temp1,&source[8*i], 8);
        des( (unsigned char*)temp1, (unsigned char*)temp2);
        memcpy(&tempSource[8*i], temp2, 8);
    }
    
    strncpy( desDest, tempSource, 8*i);
    desDest[8*i] = 0x0;

    Base64 base;
    unsigned int nOutSize;
    char *output = new char[ nSource+9];
    //output[nSource+9] = 0x0;
    int iOutput=0;
    patch( tempSource, (char*)pKey, nSource, output, iOutput);
    output[nSource+8] = 0x0;
    unsigned char *ptemp = base.operate_buf( (int)'e', (unsigned char*)output,(unsigned int) nSource+8, nOutSize);
    len = ((nSource+7)/3)*4 + 6;
    strncpy( dest, (char *)ptemp, len);
    dest[len] = 0x0;
    delete []tempSource;
    delete []ptemp;
    delete[]output;
    return len;
}


int DesDecryptFixKey(const char* source, char*dest)
{
    int len;
    char *pTempSource = NULL;
    pTempSource = new char[strlen(source)+2];
    if(NULL == pTempSource)
    {
       return 0;
    }
    memset(pTempSource, 0, strlen(source)+2);
    memcpy(pTempSource, source, strlen(source));
    pTempSource[strlen(source)] = 13;
    

    /// 解码
    int nSource = (int)strlen(pTempSource);
    unsigned int nOutSize;
    Base64 base;
    unsigned char *ptemp = NULL;
    ptemp =  base.operate_buf( 'd', (unsigned char*)pTempSource, nSource, nOutSize);
    
    //strncpy( dest, ptemp, nOutSize);

    /// 抽取密钥
    unsigned char *key = NULL;
    key  = new unsigned char[8];
    if(NULL == key)
    {
        if(NULL != ptemp)
        {
            delete [] ptemp;
            ptemp =  NULL;
        }

        if(NULL != pTempSource)
        {
           delete []pTempSource;
	 pTempSource = NULL;
        }
        return 0;
    }
    memset(key, 0, 8);
    nSource = ((nSource-6)/4)*3 + 3;
    nSource =  nSource -nSource%8 + 8;
    int n = (nSource-8)/8;

    char *output = NULL;
    output = new char[ nSource-8];
    if(NULL == output)
    {
        if(NULL != ptemp)
        {
            delete [] ptemp;
            ptemp =  NULL;
        }
        
        if(NULL != key)
        {
            delete [] key;
            key = NULL;
        }
        if(NULL != pTempSource)
        {
            delete []pTempSource;
            pTempSource = NULL;
        }
        return 0;   
    }
    memset(output, 0, nSource-8);
    int iOutput =0;

    if(detach( (char*)ptemp, (char*)key, nOutSize, output, iOutput) == 0)
    {
        if(NULL != output)
        {
            delete []output;
            output = NULL;
        }
        if(NULL != ptemp)
        {
            delete []ptemp;
            ptemp = NULL;
        }

        if(NULL != key)
        {
            delete []key;
	  key = NULL;
        }
        if(NULL != pTempSource)
        {
            delete []pTempSource;
            pTempSource = NULL;
        }

        dest[0] = 0x0;
        return 0;
    }

    //pKey[8] = 0x0;
    deskey( key, DE1);
    char *tempSource = NULL;
    tempSource = new char[ nSource-8];

    if(NULL == tempSource)
    {
        if(NULL != output)
        {
            delete []output;
            output = NULL;
        }
        if(NULL != ptemp)
        {
            delete []ptemp;
            ptemp = NULL;
        }

        if(NULL != key)
        {
            delete []key;
	  key = NULL;
        }
        if(NULL != pTempSource)
        {
            delete []pTempSource;
            pTempSource = NULL;
        }
        return 0;
    }

    memset(tempSource, 0, nSource-8);
    len = (int)iOutput;
    int i;
    for ( i=0; i< n; i++)
    {
        char temp1[8];
        char temp2[8];
        for ( int j=0; j<8; j++)
        {
            temp1[j] = 0x0;
            temp2[j] = 0x0;
        }
        memcpy( temp1,&output[8*i], 8);
        des( (unsigned char*)temp1, (unsigned char*)temp2);
        strncpy( &tempSource[8*i], temp2, 8);

    }
//des( (unsigned char*)output, (unsigned char*)tempSource);
    strncpy ( dest, tempSource, iOutput);	
    dest[ iOutput] = 0x00;
    if(NULL != tempSource)
    {
        delete []tempSource;
        tempSource = NULL;
    }

    if(NULL != ptemp)
    {
        delete []ptemp;
        ptemp = NULL;
    }

    if(NULL != pTempSource)
    {
        delete []pTempSource;
        pTempSource = NULL;
    }

    if(NULL != output)
    {
        delete []output;
        output = NULL;
    }

    return len;
}



