#pragma once
#include "Common.h"

///************CSHString结束******************
////////////////////////////////////////////////////////////////////////////////
/// Class Name  : CEncryptDecrypt
/// Description : 加解密处理类
/// See Also    : @see 
///************CEncryptDecrypt开始******************
class SH_EXPORT CSHEncrypt
{
public:
    /**
      * 该方法提供了对称加解密的功能。	 
      * @param Type[in]：确认操作类型是加密还是解密 
      *  0表示加密对称 1 表示对称解密
      * @param pInput[in]：输入的原文 
      * @param lenInput[in]：输入原文的长度 
      * @param pOutput[in]：输出密文的buffer,此buffer需要调用者在调用接口前分配并初始化过 
      * @param lenOutput[out]：输出数据长度 
      * @param CryType[in]：加密算法类型，默认为3des算法 
      *                    0表示的是des算法    1表示的是3des算法
      * @param pkeyCh[in]：密钥，默认为12345678 
      * @return 0 成功 其他值失败
      * @exception 无
      * @see 
      */
      static int32 EncryptDecryptData(int32 type, const char* pInput, int32 lenInput, char *pOutput, int32 *lenOutput, int32 cryType=1, const char* pKeyCh="01234567");
    /**
      * 该方法提供了固定密钥的对称加解密的功能,默认的算法是3des算法。	 
      * @param pInput[in]：输入的原文 
      * @param pOutput[in]：接收密文的buffer,此buffer需要调用者在调用之前分配并初始化过 
      * @param CryType[in]：加密算法类型，默认为3des算法 
      *                    0表示的是des算法    1表示的是3des算法
      * @return 0 成功 其他值失败
      * @exception 无
      * @see 
      */
      static int32 TripleDesFixKeyEncryptDecryptData(int32 type, const char* pInput, char *pOutput, int32 cryType=1);	
    /**
      * 该方法提供的功能对一段数据进行摘要,默认的是MD5摘要.摘要后的数据被base64编码.
      * @param  pOriginalText[in]：输入的原文
      * @param  lenOriginalText[in]：输入的原文长度 
      * @param  pDigestInfo[in]：接收原文的摘要信息的buffer
      * @param  lenDigest[out]： 摘要信息的长度
      * @param  digestArithType[in]： 摘要的算法类型目前提供Md5算法，默认的是为0即Md5算法 
      * @return 0 成功 其他值失败
      * @exception 无
      * @see 
      */
      static int32 DigestInfo(const char* pOriginalText, int32 lenOriginalText, char *pDigestInfo, int32 *lenDigest,int32 digestArithType=0);

	  static string Encrypt(string minwen);
	  static string Decrypt(string miwen);
};