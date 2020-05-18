#ifndef DESBASE64_H
#define DESBASE64_H

/**
  * 
  * @param pKey[in]:
  * @param source[in]:
  * @param dest[in]:
  * @param desDest[in]:
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */
extern int DesEncrypt(const char *pKey,  const char* source, char*dest, char *desDest);

/**
  * 
  * @param pKey[in]:
  * @param source[in]:
  * @param dest[in]:
  * @param desDest[in]:
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */

extern int DesDecrypt(const char *pKey, const char* source, char*dest, char *desDest);

/**
  * 
  * @param source[in]:
  * @param dest[in]:
  * @param desDest[in]:
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */

extern int DesEncryptFixKey(const char* source, char*dest);

/**
  * 固定秘要解密算法 
  * @param  source[in]:
  * @param  dest[in]:
  * @param  desDest[in]:
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */

extern int DesDecryptFixKey(const char* source, char*dest);

#endif  /// desBase64.h end 


