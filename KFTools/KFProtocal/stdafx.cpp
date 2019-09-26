
// stdafx.cpp : 只包括标准包含文件的源文件
// KFProtocal.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "KFrame.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #pragma comment(lib, "Advapi32.lib")
    #pragma comment(lib, "User32.lib")
    #pragma comment(lib,"iphlpapi.lib")
    #pragma comment(lib, "libprotobufd.lib")
    #pragma comment(lib, "libprotocd.lib" )
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_DEBUG__
        #pragma comment( lib, "KFPlugind.lib" )
        #pragma comment( lib, "KFMemoryd.lib" )
        #pragma comment( lib, "KFUtilityd.lib" )
        #pragma comment( lib, "KFGlobald.lib" )
        #pragma comment( lib, "KFLoggerd.lib" )
        #pragma comment( lib, "KFExceld.lib" )
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_RELEASE__
        #pragma comment( lib, "KFPlugin.lib" )
        #pragma comment( lib, "KFMemory.lib" )
        #pragma comment( lib, "KFUtility.lib" )
        #pragma comment( lib, "KFGlobal.lib" )
        #pragma comment( lib, "KFLogger.lib" )
        #pragma comment( lib, "KFExcel.lib" )
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
#endif
