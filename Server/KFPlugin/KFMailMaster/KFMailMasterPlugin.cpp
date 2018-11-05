#include "KFMailMasterPlugin.h"
#include "KFMailMasterModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailMasterPlugin::Install ()
    {
        __REGISTER_MODULE__( KFMailMaster );
    }

    void KFMailMasterPlugin::UnInstall ()
    {
        __UNREGISTER_MODULE__( KFMailMaster );
    }

    void KFMailMasterPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        //__FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
    }
}