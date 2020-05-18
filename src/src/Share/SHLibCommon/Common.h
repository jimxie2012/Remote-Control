#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <stdarg.h>
 
#define uint32     unsigned int
#define uint16     unsigned short
#define int32      int
typedef unsigned char  uint8;
typedef char           int8;
#ifdef _LINUX
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#if defined (WIN32)
#define SH_EXPORT __declspec(dllexport)
#else 
#define SH_EXPORT 
#endif

using namespace std;