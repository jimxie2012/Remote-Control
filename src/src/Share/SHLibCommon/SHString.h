#pragma once
#include "Common.h"
////////////////////////////////////////////////////////////////////////////////
/// Class Name  : CSHString
/// Description : 字符串处理类
/// See Also    : @see 
///************CSHString开始******************

class SH_EXPORT CSHString
{
public:

  /**
    * <P>用strReplace替换str中第一个sFind<P>
    * @param str:[in]原始string
    * @param sFind:[in]
    * @param sReplease:[in]
    * @param sReturn:[out]得到的子string
    * @return void
    * @exception
    */
    static void strReplace(string str,string strFind,string strReplace,string &sReturn);
    /**
    * <P>用strRemoveSpace去掉字符串中空格<P>
    * @param str:[in]原始string
    * @param sReturn:[out]得到的子string
    * @return void
    * @exception
    */
	static void strRemoveSpace(string str,string &sReturn);
    /**
    * <P>得到str中sKey的值(sKey=sReturn;)<P>
    * @param str:[in]原始string
    * @param sKey:[in]
    * @param s1:[in]
    * @param s2:[in]
    * @param sReturn:[out]得到的子string
    * @return void
    * @exception
    */
    static bool GetStrValue(string str,string sKey,string &sReturn,string s1=";",string s2="=");
    static bool SetStrValue(string str,string sKey,string newData,string &sReturn,string s1,string s2="=");
    /**
    * <P>str格式化<P>
    * @param str:[in]原始string
    * @param sReturn:[out]得到的子string
    * @return void
    * @exception
    */   
    static void strFormat(string &sReturn,string str,...);
	
    /**
    * <P>str格式化<P>
    * @param str:[in]原始string
    * @param arglist:[in]va_list
    * @param sReturn:[out]得到的子string
    * @return void
    * @exception
    */   
    static void strFormatEx(string &sReturn,string str,va_list arglist);
    
    /**
    * <P>得到sStart与sEnd之间的子string 最左边查找<P>
    * @param str:[in]原始string
    * @param sStart:[in]开始string
    * @param sEnd:[in]结束string
    * @param sReturn:[out]得到的子string
    * @return bool: 成功/失败
    * @exception
    */
    static bool GetLeftSubStr(string str,string sStart,string sEnd,string &sReturn);

    /**
    * <P>得到sStart与sEnd之间的子string 最右边查找<P>
    * @param str:[in]原始string
    * @param sStart:[in]开始string
    * @param sEnd:[in]结束string
    * @param sReturn:[out]得到的子string
    * @return bool: 成功/失败
    * @exception
    */
    static bool GetRightSubStr(string str,string sStart,string sEnd,string &sReturn);

    /**
    * <P>得到sStart与sEnd之间的子string 开始最左，结束最右<P>
    * @param str:[in]原始string
    * @param sStart:[in]开始string
    * @param sEnd:[in]结束string
    * @param sReturn:[out]得到的子string
    * @return bool: 成功/失败
    * @exception
    */
    static bool GetLeftRightSubStr(string str,string sStart,string sEnd,string &sReturn);

    /**
    * <P>把以sFind分割的str存储在VEC_str中<P>
    * @param vecStr: [in,out]存储分割结果的vector
    * @param str: [in]以ch分割的string
    * @param sFind: [in]分割符
    * @return     
    * @exception
    */
    static void ConvertString2Vector(vector<string>  &vecStr,string str,string sFind=",");

    /**
    * <P>比较2个string<P>
    * @param str1: [in]第一个string
    * @param str2: [in]第二个string
    * @param bCase: [in]是否区分大小写
    * @return 比较结果
    <0：str1<str2
    =0：str1=str2
    >0：str1>str2
    * @exception
    */
    static int CmpStr(string str1,string str2,bool bCase=false);
    
    /**
    * <P>去除string左边的字符<P>
    * @param str: [in,out]要转换的string
    * @param c: [in]要去除的字符
    * @return void
    * @exception
    */
    static void TrimCharLeft(string &str,char c);
    
    /**
    * <P>去除string右边的字符<P>
    * @param str: [in,out]要转换的string
    * @param c: [in]要去除的字符
    * @return void
    * @exception
    */
    static void TrimCharRight(string &str,char c);
    
    /**
    * <P>去除string两边的字符<P>
    * @param str: [in,out]要转换的string
    * @param c: [in]要去除的字符
    * @return void
    * @exception
    */
    static void TrimChar(string &str,char c);

    /** 
      * 去除字符串中的空格字符
      * @param s[in]:输入字符串
      * @return 剪裁后的字符串
      * @exception 无
      * @see  
      */
    static string Trim(const string& s); 
	
    /**
      * 把一个字符串按指定的字符分割成几个字符串，并把分割后的字符串存放在set里
      * @param str[in]:输入字符串
      * @param separator[in]:判断分割的字符
      * @return 分割之后的一组字符串并放在set中
      * @exception 无
      * @see  
      */
    static std::set<string> Split(string str,char separator);

      
      /** 把一个字符串按照特定的分割字符分割成几个字符串，不取出各子串中任何空格及控制符
      * 分割后的所有子串保存在vector的string里。返回vector的大小固定为
      * 输入字符串中指定分隔符出现的次数加一
      * @param str[in]: 输入字符串
      * @param separator[in]:判断分割的字符
      * @return 返回vector，vector里存放的事分割后的字符串  
      * @exception 无
      * @see  
      */
    static vector<string> SplitAllInVector(string str,char separator);

    /** 
      * 把一个字符串按照特定的分割字符分割成几个字符串，并去除各子串的前后空格及控制符
      * 分割后的所有子串保存在vector的string里，空字符串也一并保留。返回vector的大小固定为
      * 输入字符串中指定分隔符出现的次数加一
      * @param str[in]: 输入字符串
      * @param separator[in]:判断分割的字符
      * @return 返回vector，vector里存放的事分割后的字符串  
      * @exception 无
      * @see  
      */
    static vector<string> SplitAllInVectorNoTrim(string str,char separator);

        /** 
      * 把一个字符串按照特定的分割字符分割成几个字符，并把分割后的字符串放在vector的string里	      
      * 同时删除所有空字符串
      * @param str[in]: 输入字符串
      * @param separator[in]:判断分割的字符
      * @return 返回vector，vector里存放的事分割后的字符串  
      * @exception 无
      * @see  
      */
    static vector<string> SplitInVector(string str,char separator);
	
    /** 
      * 把一个字符串按照特定的分割字符分割成几个字符，并把分割后的字符串放在vector的string里，
      * 对每个字符串不进行Trim操作	      
      * @param str[in]: 输入字符串
      * @param separator[in]:判断分割的字符
      * @return 返回vector，vector里存放的事分割后的字符串  
      * @exception 无
      * @see  
      */
    static vector<string> SplitInVectorNoTrim(string str,char separator);
	
    /**
      * <P>csv文件字符串拆分</P> 
      * @param str: [in]需拆分的一行csv格式字符串
      * @return 已拆分好的各字段内容
      */
    static vector<string> SplitCsvFields(string& str);
        
    /**
      * 取两组string形式的set相对应的string的子集
      * @param r1[in]：输入的字符串
      * @param r2[in]：输入的字符串 
      * @return 返回set形式的string，两个输入字符串的交集放在放在string里  
      * @exception 无
      * @see
      */
      static std::set<string> StringSetSubstraction(std::set<string> &r1,std::set<string> &r2);
	
      ///implements the string sets intersection
    /**
      * 取两组字符串的交集，并把交集的字符串放到set形式的string中
      * @param r1[in]：输入的字符串
      * @param r2[in]：输入的字符串 
      * @return 转换后的32位无符号整数
      * @exception 无
      * @see
      */	
      static std::set<string> StringSetIntersection(std::set<string> &r1,std::set<string> &r2);

    /**
      * 将theString转换为大写形式
      * @param theString[in]：输入的字符串
      * @return 转换后的32位无符号整数
      * @exception 无
      * @see
      */	    
      static string ToUpperCase(const string theString);
	
    /**
      * 将theString转换为小写形式
      * @param theString[in]：输入的字符串
      * @return 转换后的32位无符号整数
      * @exception 无
      * @see
      */
      static string ToLowerCase(const string theString);
	
    /**
      * 判断字符串是否是数字形式
      * @param theString[in]：
      * @return 
      * @exception 无
      * @see
      */
      static bool IsDigit(const string theString);   	
    /**
      * 
      * @param pBuf：要转换字符串的首地址
      * @param nLen：要转换字符串的长度 
      * @return 转换后的32位无符号整数
      * @exception logic_error
      * @see
      */
      static uint32 StringToLong(const char* pBuf, int nLen);
	
    /**
      * 
      * @param pBuf：要转换字符串的首地址
      * @param nLen：要转换字符串的长度 
      * @return 转换后的16位无符号整数
      * @exception logic_error
      * @see
      */
      static uint16 StringToShort(const char* pBuf, int nLen) ;	    
    /**
      * 将字符串中的指定 char 用指定 string 替换
      * @param strOrig  ：原字符串
      * @param chKey    ：需要被替换的 char
      * @param strValue ：用来替换 char 的 string
      * @return string  ：替换过后产生的字符串
      * @exception 无
      * @see
      */
    static string ReplaceCharWithString(const string& strOrig, const char chKey, const string& strValue );     

    static std::string Base64Encode(string str);
	static std::string Base64Decode(string str);
};
