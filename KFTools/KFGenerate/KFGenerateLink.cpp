#include "KFGenerateLogic.h"

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"winhttp.lib")
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KF_DEBUG__
    #pragma comment( lib, "KFMemoryd.lib" )
    #pragma comment( lib, "KFUtilityd.lib" )
    #pragma comment( lib, "KFGlobald.lib" )
    #pragma comment( lib, "KFExceld.lib" )
    #pragma comment( lib, "KFLoggerd.lib" )
    #pragma comment( lib, "git2d.lib" )
    #pragma comment( lib, "libssh2d.lib" )
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KF_RELEASE__
    #pragma comment( lib, "KFMemory.lib" )
    #pragma comment( lib, "KFUtility.lib" )
    #pragma comment( lib, "KFGlobal.lib" )
    #pragma comment( lib, "KFExcel.lib" )
    #pragma comment( lib, "KFLogger.lib" )
    #pragma comment( lib, "git2.lib" )
    #pragma comment( lib, "libssh2.lib" )
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
