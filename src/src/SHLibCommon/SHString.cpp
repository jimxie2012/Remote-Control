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

#include "SHString.h"
#include "CBase64.h"
#include "desBase64.h"
#include <fstream>
#include <iostream>  
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
template<typename T> T fromString(const std::string& s) 
{
    std::istringstream is(s);
    T t;

    is >> t;
    return t;
}

template<typename T> std::string toString(const T& t) 
{
    std::ostringstream s;
    s << t;
    return s.str();
}
  

void CSHString::ConvertString2Vector(vector<string> &vecStr, string str, string sFind)
{
    int pos1=0;
    int pos2=0;
    vecStr.clear();
    while((pos2=(int)str.find(sFind,pos1))!=(int)string::npos)
    {
        vecStr.push_back(str.substr(pos1,pos2-pos1));
        pos1=pos2+(int)sFind.length();
    }
    string strTemp = str.substr(pos1);	
    if(strTemp.size()>0)
    {
        vecStr.push_back(str.substr(pos1));
    }
}
bool CSHString::GetLeftSubStr(string str, string sStart, string sEnd,string &sReturn)
{
    sReturn="";
    int nStart=(int)str.find(sStart);
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    int nEnd=(int)str.find(sEnd,nStart+sStart.size());
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    sReturn=str.substr(nStart+sStart.size(),nEnd-(nStart+sStart.size()));
    return true;
}

bool CSHString::GetRightSubStr(string str, string sStart, string sEnd,string &sReturn)
{
    sReturn="";
    int nStart=(int)str.rfind(sStart);
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    int nEnd=(int)str.rfind(sEnd);
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    sReturn=str.substr(nStart+sStart.size(),nEnd-(nStart+sStart.size()));
    return true;
}

bool CSHString::GetLeftRightSubStr(string str, string sStart, string sEnd,string &sReturn)
{
    sReturn="";
    int nStart=(int)str.find(sStart);
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    int nEnd=(int)str.rfind(sEnd);
    if((unsigned)nStart==str.npos)
    {
        return false;
    }
    sReturn=str.substr(nStart+sStart.size(),nEnd-(nStart+sStart.size()));
    return true;
}

void CSHString::TrimCharLeft(string &str, char c)
{
    size_t i=0;    
    while(i<str.size())
    {
        if(str[i]==c)
        {
            i++;
        }
        else
        {
            break;
        }
    }
    str=str.substr(i);
}

void CSHString::TrimCharRight(string &str, char c)
{
    int i=(int)str.size()-1;    
    while(i>=0)
    {
        if(str[i]==c)
        {
            i--;
        }
        else
        {
            break;
        }
    }
    str=str.substr(0,i+1);
}

void CSHString::TrimChar(string &str, char c)
{
    TrimCharLeft(str,c);
    TrimCharRight(str,c);
}

int CSHString::CmpStr(string str1, string str2,bool bCase)
{
    string s1,s2;
    s1=str1;
    s2=str2;
    if(bCase)
    {
       // LowStr(s1);
        //LowStr(s2);
    }
    return s1.compare(s2);
}


void CSHString::strFormat(string &sReturn,string str,...)
{
    char buff[1024];    
    memset(buff, 0, 1024);
    va_list    arglist;
    va_start( arglist, str );
    #if defined(WIN32) 
    _vsnprintf( buff, sizeof(buff), str.c_str(), arglist );
    #else
    vsnprintf( buff, sizeof(buff), str.c_str(), arglist );
    #endif
    va_end( arglist );
    sReturn.clear();
    sReturn=buff;
 
}

void CSHString::strFormatEx(string &sReturn,string str,va_list arglist)
{
    char buff[1024];    
    memset(buff, 0, 1024);
    
    #if defined(WIN32) 
    _vsnprintf( buff, sizeof(buff), str.c_str(), arglist );
    #else
    vsnprintf( buff, sizeof(buff), str.c_str(), arglist );
    #endif
    sReturn=buff;
}

bool CSHString::GetStrValue(string str,string sKey,string &sReturn,string s1,string s2)
{
    vector<string> vStr,vecTmp2;
    ConvertString2Vector(vStr,str,s1);
    for(size_t i=0;i<vStr.size();i++)
    {
        vecTmp2.clear();
        ConvertString2Vector(vecTmp2,vStr[i],s2);
        if(vecTmp2.size()==2)
        {
            if(sKey==vecTmp2[0])
            {
                sReturn=vecTmp2[1];
                return true;
            }
        }
    }
	sReturn="";
    return false;
}
bool CSHString::SetStrValue(string str,string sKey,string newData,string &sReturn,string s1,string s2)
{
    vector<string> vStr,vecTmp2;
    ConvertString2Vector(vStr,str,s1);
	bool  ret = false;
    for(size_t i=0;i<vStr.size();i++)
    {
        vecTmp2.clear();
        ConvertString2Vector(vecTmp2,vStr[i],s2);
        if(vecTmp2.size()==2)
        {
            if(sKey==vecTmp2[0])
            {
                vecTmp2[1]=newData;
				vStr[i] = sKey+s2+vecTmp2[1];
				ret = true;
            }
		    if (vStr[i]!="")
		       vStr[i] = vStr[i] + s1;
        }
    }   
	sReturn="";
	for(size_t i=0;i<vStr.size();i++)
    {  
		vecTmp2.clear();
        ConvertString2Vector(vecTmp2,vStr[i],s2);
        if(vecTmp2.size()==2)
        {
		    sReturn = sReturn+vStr[i];
        }
    }
	if (!ret)
		sReturn = sReturn + sKey + s2+newData+s1;
    return true;
}
void CSHString::strReplace(string str, string strFind, string strReplace,string &sReturn)
{
    sReturn=str;
    int nPos=(int)sReturn.find(strFind);
    if(nPos<0)
    {
        return;
    }
    sReturn.replace(nPos,strFind.size(),strReplace);
}


string CSHString::Trim(const string& s) 
{
    
    if ( s.length() == 0 )
        return s;
    int beg = (int)s.find_first_not_of(" \a\b\f\n\r\t\v");
    int end = (int)s.find_last_not_of(" \a\b\f\n\r\t\v");
    if ( beg == (int)std::string::npos ) // No non-spaces
        return "";
    return string(s, beg, end - beg + 1);

}

set<string> CSHString::Split(string str,char separator)
{
    set<string> results;
	
    size_t len = str.length();
	
    int begin = 0, length  = 0;
	
    for ( unsigned int i = 0 ;i <= len; i++,length++ )
    {
        if ( str[i] == separator || i == len )
        {
            string tmp;
            if ( i == len )
                tmp = str.substr(begin,length+1);
            else
                tmp = str.substr(begin,length);
            begin = i+1;
            i++;
            length = 0;
            tmp = Trim(tmp);
            if ( tmp == "" )
            {
                continue;
            }
            if ( 1 == tmp.size() && tmp[0] == separator )
            {
                continue;
            }
			
            results.insert(tmp);
        }
    }
    return results;
}

vector<string> CSHString::SplitAllInVector(string str,char separator)
{
    vector<string> results;
    int begin = 0;
    string tmp = "";

    size_t len = str.length();        
    if(len == 0) return results;
   
    unsigned int i;
    for ( i = 0 ;i < len; i++ )
    {
        if(str[i] == separator)
        {
            tmp = str.substr(begin, i - begin);
            results.push_back(Trim(tmp));

            begin = i + 1;
        }
    }

    /// post process
    if(str[len - 1] == separator)
        results.push_back("");
    else
    {
        tmp = str.substr(begin, i - begin + 1);
        results.push_back(Trim(tmp));
    }
    
    return results;
}

vector<string> CSHString::SplitAllInVectorNoTrim(string str,char separator)
{
    vector<string> results;
    int begin = 0;
    string tmp = "";

    size_t len = str.length();        
    if(len == 0) return results;
    
    unsigned int i;

    for ( i = 0 ;i < len; i++ )
    {
        if(str[i] == separator)
        {
            tmp = str.substr(begin, i - begin);
            results.push_back(tmp);

            begin = i + 1;
        }
    }

    /// post process
    if(str[len - 1] == separator)
        results.push_back("");
    else
    {
        tmp = str.substr(begin, i - begin + 1);
        results.push_back(tmp);
    }
    
    return results;
}

vector<string> CSHString::SplitInVector(string str,char separator)
{
    vector<string> results;
	
    size_t len = str.length();
	
    int begin = 0, length  = 0;
	
    for ( unsigned int i = 0 ;i <= len; i++,length++ )
    {
        if ( str[i] == separator || i == len )
        {
            string tmp;
            if ( i == len )
                tmp = str.substr(begin,length+1);
            else
                tmp = str.substr(begin,length);
            begin = i+1;
            i++;
            length = 0;
            tmp = Trim(tmp);
            if ( tmp == "" )
            {
                continue;
            }
			
            if ( 1 == tmp.size() && tmp[0] == separator )
            {
                continue;
            }
            results.push_back(tmp);
        }
    }
    return results;

}


vector<string> CSHString::SplitInVectorNoTrim(string str,char separator)
{
    vector<string> results;
	
    size_t len = str.length();
	
    int begin = 0, length  = 0;
	
    for ( unsigned int i = 0 ;i <= len; i++,length++ )
    {
        if ( str[i] == separator || i == len )
        {
            string tmp;
            if ( i == len )
                tmp = str.substr(begin,length+1);
            else
                tmp = str.substr(begin,length);
            begin = i+1;
            i++;
            length = 0;
            //tmp = Trim(tmp);
            if ( tmp == "" )
            {
                continue;
            }
			
            if ( 1 == tmp.size() && tmp[0] == separator )
            {
                continue;
            }
            results.push_back(tmp);
        }
    }
    return results;

}


vector<string> CSHString::SplitCsvFields(string& str)
{
    vector<string> results;
    /// 标识该字段是否为字符型字段
    bool bIsString = false;
    /// 字符串扫描计数器
    int count = 0;
    int begin = 0;
    string strField;

    while((unsigned)count != str.size())
    {
        /// 开始识别一个新字段
        strField = "";
        /// 过滤开头的空格
        while(str[count] == ' ') count++;
        
        begin = count;
        /// 判断第一个非空字符是否为双引号，确定该字段是否为字符型字段
        if(str[count] == '\"')
        {
            bIsString = true;
            /// 跳过开头双引号
            count++;
            begin++;
        }
        else
            bIsString = false;
        
        if(!bIsString)
        {
            /// 非字符型字段，扫描至第一个逗号结束
            while(str[count] != ',' && (unsigned)count != str.size())
                count++;
            
            /// 非空字段，保存结果
            if(count != begin)
            {
                strField = str.substr(begin, count - begin);
                results.push_back(Trim(strField));
            }
            //// 王伟修改 20070209 不跳过为空的字段
            else
            {
                results.push_back("");
            }

            /// 跳过逗号分隔符
            if(str[count] == ',')
                count++;
        }
        else
        {
            /// 字符型字段，扫描至第一个非连续出现的双引号结束
            while((unsigned)count != str.size())
            {
                if(str[count] != '\"') 
                {
                    /// 非双引号字符，继续扫描
                    count++;
                    continue;
                }

                /// 扫描到双引号，检查下一字符是否仍是双引号
                if(str[count+1] == '\"')
                {
                    /** 下一字符仍是双引号，为字符串内部双引号转义
                        跳过一个双引号     */
                    strField = strField + str.substr(begin, count - begin + 1);
                    count += 2;
                    begin = count;
                    continue;
                }
                else
                {
                    /** 下一字符不是双引号，本字段结束，
                        扫描至逗号分隔符或字符串结束  */
                    /// 保存结果
                    strField = strField + str.substr(begin, count - begin);

                    results.push_back(Trim(strField));
                    /// 扫描至逗号分隔符
                    count++;
                    while(str[count] != ',' && (unsigned)count != str.size())
                    {
                        count++;
                    }

                    /// 跳过逗号分隔符
                    if(str[count] == ',')
                        count++;
                    /// 完成一个字符型字段扫描
                    break;
                } /// end of if(str[count+1] == '\"')
            } /// end of internal while loop
        } /// end of if(!bIsString)        
    } /// end of while(count != str.size())

    return results;
}
	
	///implements the string sets substraction
set<string> CSHString::StringSetSubstraction(set<string> &r1,set<string> &r2)
{
    set<string> ret;
    set<string>::iterator it1 = r1.begin();
    for ( ; it1 != r1.end() ; it1++ )
    {
        bool flag = false;
        set<string>::iterator it2 = r2.begin();
        for ( ; it2 != r2.end() ; it2++ )
        if ( *it1 == *it2 )
        {
            flag = true;
            break;
        }
        
        if ( !flag )
            ret.insert(*it1);
    }
    
    return ret;
}
	
///implements the string sets intersection
set<string> CSHString::StringSetIntersection(set<string> &r1,set<string> &r2)
{
    set<string> ret;
    set<string>::iterator it1 = r1.begin();
    for ( ; it1 != r1.end() ; it1++ )
    {
        set<string>::iterator it2 = r2.begin();
        for ( ; it2 != r2.end() ; it2++ )
        {
            if ( *it1 == *it2 )
            {
                ret.insert(*it1);
            }
        }
    }
    return ret;

}
	
	/// 将theString转换为大写形式
string CSHString::ToUpperCase(const string theString)
{
    string ret = "";
	
    for ( size_t i = 0 ; i < theString.length() ; i++ )
    {
        if ( theString.at(i) >= 'a' && theString.at(i) <= 'z' )
        {
            ret += theString.at(i)-0x20;
        }
        else
        {
            ret += theString.at(i);
        }
    }
    return ret;
}
	
	/// 将theString转换为小写形式
string CSHString::ToLowerCase(const string theString)
{
    string ret = "";
	
    for ( size_t i = 0 ; i < theString.length() ; i++ )
    {
        if ( theString.at(i) >= 'A' && theString.at(i) <= 'Z' )
        {
            ret += theString.at(i)+0x20;
        }
        else
        {
            ret += theString.at(i);
        }
    }
    return ret;

}
	
	/// 判断字符串是否是数字形式
bool CSHString::IsDigit(const string theString)
{

    int decimalNR = 0;
    string tmp = theString;
	
    if ( 0 == theString.length() )
    {
        return false;
    }
    if ( '-' == theString.at(0) || '+' == theString.at(0) )
    {
        tmp = theString.substr(1,theString.length()-1);
    }
    if ( 0 == tmp.length() )
    {
        return false;
    }
    for ( size_t i = 0 ; i < tmp.length() ; i++ )
    {
        if ( !isdigit(tmp.at(i)) && tmp.at(i) != '.' )
        {
            return false;
        }
        
        if ( '.' == tmp.at(i) )
        {
            decimalNR++;
        }
    }
    if ( decimalNR > 1 )
    {
        /// 小数点个数非法
        return false;
    }
    return true;

}

uint32 CSHString::StringToLong(const char* pBuf, int nLen)
{
    /// 检查是否为合法的数字字符串
    for ( int i = 0; i < nLen; i++ )
    {
        if ( pBuf[i]>'9' || pBuf[i]<'0' )
        {
            /// 该字符不是一个数字字符,抛出异常
            throw logic_error("字符串不是一个合法的整数!");
        }
    }
	
    /// 字符串转换
    string strDigit(pBuf, nLen);
    return fromString<uint32>(strDigit);

}
	

uint16 CSHString::StringToShort(const char* pBuf, int nLen)
{
    /// 检查是否为合法的数字字符串
    for ( int i = 0; i < nLen; i++ )
    {
        if ( pBuf[i]>'9' || pBuf[i]<'0' )
        {
            /// 该字符不是一个数字字符,抛出异常
            throw logic_error("字符串不是一个合法的整数!");
        }
    }
	
    /// 字符串转换
    string strDigit(pBuf, nLen);
    return fromString<uint16>(strDigit);

}

/// Added at 20070817
    /**
      * 将GB2312编码的字符串转换为UTF8编码
      * @param szSrc：GB2312编码的字符串
      * @return char * ： UTF8 编码的字符串，使用后需用 free() 调用释放动态分配的内存
      * @exception 无
      * @see
      */
/*      
    char * CSHString::GB2312ToUTF8( const char * szSrc )
    {
        /// GB2312 to UNICODE
        int wLen = MultiByteToWideChar( CP_ACP, 0, szSrc, -1, NULL, 0 );
        LPWSTR wStr = (LPWSTR)malloc( sizeof( WCHAR ) * wLen );
        MultiByteToWideChar( CP_ACP, 0, szSrc, -1, wStr, wLen );
        
        /// UNICODE to UTF8
        int aLen = WideCharToMultiByte( CP_UTF8, 0, wStr, -1, NULL, 0, NULL, NULL );
        char * szDest = (char*)malloc( sizeof( char ) * aLen );
        WideCharToMultiByte( CP_UTF8, 0, wStr, -1, szDest, aLen, NULL, NULL );
        
        free( wStr );
        
        return( szDest );
    }
*/    

/// End of adding - 20070817

/// Added at 20071019
    /**
      * 将字符串中的指定 char 用指定 string 替换
      * @param strOrig  ：原字符串
      * @param chKey    ：需要被替换的 char
      * @param strValue ：用来替换 char 的 string
      * @return string  ：替换过后产生的字符串
      * @exception 无
      * @see
      */
    string CSHString::ReplaceCharWithString(const string& strOrig, const char chKey, const string& strValue )
    {
        string strRet;

        for ( size_t i = 0 ; i < strOrig.length() ; i ++ )
        {
            if ( strOrig.at(i) == chKey )
            {
                strRet += strValue;
            }
            else
            {
                strRet += strOrig.at(i);
            }
        }

        return strRet;
    }

void CSHString::strRemoveSpace(string str,string &sReturn)
{
    string tmp = str;
    while((strstr(tmp.c_str()," ") )!=NULL)
    {
        CSHString::strReplace(tmp," ","",tmp);
    }
    sReturn = tmp; 
}
static const std::string base64_chars =   
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
             "abcdefghijklmnopqrstuvwxyz"  
             "0123456789+/";  
  
  
static inline bool is_base64(unsigned char c) {  
  return (isalnum(c) || (c == '+') || (c == '/'));  
}  
  
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {  
  std::string ret;  
  int i = 0;  
  int j = 0;  
  unsigned char char_array_3[3];  
  unsigned char char_array_4[4];  
  
  while (in_len--) {  
    char_array_3[i++] = *(bytes_to_encode++);  
    if (i == 3) {  
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
      char_array_4[3] = char_array_3[2] & 0x3f;  
  
      for(i = 0; (i <4) ; i++)  
        ret += base64_chars[char_array_4[i]];  
      i = 0;  
    }  
  }  
  
  if (i)  
  {  
    for(j = i; j < 3; j++)  
      char_array_3[j] = '\0';  
  
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
    char_array_4[3] = char_array_3[2] & 0x3f;  
  
    for (j = 0; (j < i + 1); j++)  
      ret += base64_chars[char_array_4[j]];  
  
    while((i++ < 3))  
      ret += '=';  
  
  }  
  
  return ret;  
  
}  
  
std::string base64_decode(std::string const& encoded_string) {  
  int in_len = encoded_string.size();  
  int i = 0;  
  int j = 0;  
  int in_ = 0;  
  unsigned char char_array_4[4], char_array_3[3];  
  std::string ret;  
  
  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {  
    char_array_4[i++] = encoded_string[in_]; in_++;  
    if (i ==4) {  
      for (i = 0; i <4; i++)  
        char_array_4[i] = base64_chars.find(char_array_4[i]);  
  
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);  
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);  
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];  
  
      for (i = 0; (i < 3); i++)  
        ret += char_array_3[i];  
      i = 0;  
    }  
  }  
  
  if (i) {  
    for (j = i; j <4; j++)  
      char_array_4[j] = 0;  
  
    for (j = 0; j <4; j++)  
      char_array_4[j] = base64_chars.find(char_array_4[j]);  
  
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);  
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);  
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];  
  
    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];  
  }  
  
  return ret;  
}
std::string CSHString::Base64Encode(string str){
	return base64_encode((unsigned char const*)(str.c_str()), strlen(str.c_str()));
}
std::string CSHString::Base64Decode(string str){
	return base64_decode(str);
}