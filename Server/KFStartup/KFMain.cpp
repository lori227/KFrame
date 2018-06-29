#include "KFApplication.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#if __KF_SYSTEM__ == __KF_WIN__
    #pragma comment(lib, "Advapi32.lib")
    #pragma comment(lib, "User32.lib")
    #pragma comment(lib,"iphlpapi.lib")
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_DEBUG__
        #pragma comment( lib, "KFPlugind.lib" )
        #pragma comment( lib, "KFMemoryd.lib" )
        #pragma comment( lib, "KFUtilityd.lib" )
        #pragma comment( lib, "KFThreadd.lib" )
        #pragma comment( lib, "KFTimed.lib" )
        #pragma comment( lib, "KFXmld.lib" )
        #pragma comment( lib, "KFRandd.lib" )
        #pragma comment( lib, "KFLoggerd.lib" )

    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef __KF_RELEASE__
        #pragma comment( lib, "KFPlugin.lib" )
        #pragma comment( lib, "KFMemory.lib" )
        #pragma comment( lib, "KFUtility.lib" )
        #pragma comment( lib, "KFThread.lib" )
        #pragma comment( lib, "KFTime.lib" )
        #pragma comment( lib, "KFXml.lib" )
        #pragma comment( lib, "KFRand.lib" )
        #pragma comment( lib, "KFLogger.lib" )
    #endif
    //////////////////////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{
    _kf_appliction = KFrame::KFApplication::Instance();
    return _kf_appliction->run( argc, argv );
}

