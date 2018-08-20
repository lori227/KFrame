#include "KFDirProxyPlugin.h"
#include "KFDirProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFDirProxy );
    }

    void KFDirProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDirProxy );
    }

    void KFDirProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}