#ifndef MD5_H
#define MD5_H

#define PROTO_LIST(list) list

typedef unsigned int UINT4;
typedef unsigned char *POINTER;
/** UINT2 defines a two byte word */
typedef unsigned short int UINT2;

typedef struct 
{
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

/// Class Name  : 
/// Description : 
/// See Also    : @see 
/**
  * 
  * @param 
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */
  void MD5Init PROTO_LIST ((MD5_CTX *));
/**
  * 
  * @param  
  * @param 
  * @param  
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */
  void MD5Update PROTO_LIST((MD5_CTX *, unsigned char *, unsigned int));
/**
  * 
  * @param  
  * @param  
  * @return 大于 0 成功 其他值失败
  * @exception 无
  * @see 
  */
  void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));

#endif /// MD5_H end 
