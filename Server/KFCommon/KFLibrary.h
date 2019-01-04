#ifndef __KF_LIBRARY_H__
#define __KF_LIBRARY_H__
#include "KFSystem.h"

#if __KF_SYSTEM__ == __KF_WIN__
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_DEBUG__
        #pragma comment( lib, "libprotobufd.lib" )
        #pragma comment( lib, "iphlpapi.lib" )
        #pragma comment( lib, "libmysql.lib" )
        #pragma comment( lib, "mysqlclient.lib" )
        #pragma comment( lib, "hiredisd.lib" )
        #pragma comment( lib, "Win32_Interopd.lib" )
        #pragma comment( lib, "Ws2_32.lib" )
        #pragma comment( lib, "Wldap32.lib" )
        #pragma comment( lib, "libeay32.lib" )
        #pragma comment( lib, "ssleay32.lib" )

        #pragma comment( lib, "KFPlugind.lib" )
        #pragma comment( lib, "KFMemoryd.lib" )
        #pragma comment( lib, "KFUtilityd.lib" )
        #pragma comment( lib, "KFGlobald.lib" )
        #pragma comment( lib, "KFNetworkd.lib" )
        #pragma comment( lib, "KFHttpd.lib" )
        #pragma comment( lib, "KFConHashd.lib" )
        #pragma comment( lib, "KFProtocold.lib" )
        #pragma comment( lib, "LuaPlusd.lib" )
        #pragma comment( lib, "KFMath3Dd.lib" )
        #pragma comment( lib, "KFCored.lib" )
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_RELEASE__
        #pragma comment( lib, "libprotobuf.lib" )
        #pragma comment( lib, "iphlpapi.lib" )
        #pragma comment( lib, "libmysql.lib" )
        #pragma comment( lib, "mysqlclient.lib" )
        #pragma comment( lib, "hiredis.lib" )
        #pragma comment( lib, "Win32_Interop.lib" )
        #pragma comment( lib, "Ws2_32.lib" )
        #pragma comment( lib, "Wldap32.lib" )
        #pragma comment( lib, "libeay32.lib" )
        #pragma comment( lib, "ssleay32.lib" )


        #pragma comment( lib, "KFPlugin.lib" )
        #pragma comment( lib, "KFMemory.lib" )
        #pragma comment( lib, "KFUtility.lib" )
        #pragma comment( lib, "KFGlobal.lib" )
        #pragma comment( lib, "KFNetwork.lib" )
        #pragma comment( lib, "KFHttp.lib" )
        #pragma comment( lib, "KFConHash.lib" )
        #pragma comment( lib, "KFProtocol.lib" )
        #pragma comment( lib, "LuaPlus.lib" )
        #pragma comment( lib, "KFMath3D.lib" )
        #pragma comment( lib, "KFCore.lib" )
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////


#endif

