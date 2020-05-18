#pragma once
#include "Common.h"

////////////////////////////////////////////////////////////////////////////////
/// Class Name  : CLicenseTools
/// Description : license工具
/// See Also    : @see 
///************CLicenseTools开始******************
class SH_EXPORT CSHLicense
{
public:
	/**
	* 对license内容进行加密形成key
	* @param content[in]: license文件的具体内容
	* @param size[in]: license内容字符串的大小
	* @param bufferKey[out]: 保存licenseKey的字符串
	* @param bufOfLen[out]: licenseKey的大小
	* @return [int]: 0表示成功，-1表示失败
	*/  
	static int CreateLicenseKey( const char*content, const int size, char *bufferKey, int &bufOfLen);
	/**
	* 验证license是否合法
	* @param content[in]: license的内容
	* @param size[in]: license的长度
	* @param key[in]: key的内容
	* @param keysize[in]: key的长度
	* @return: 0表示验证成功,-1表示验证失败
	*/
	static int CheckLicense( const char*content, const int size, const char*key, const int keysize);
	/** 
	* @param licenseFile[in]: license文件的内容
	* @param keyFile[in]:key文件的内容
	* @return : 合法返回true, 非法返回false
	*/
	static bool CheckLicenseByFile( char* licenseFile, char*keyFile);

};
