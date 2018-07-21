#include "KFLogProxyPlugin.h"
#include "KFLogProxyModule.h"

namespace KFrame
{
    void KFLogProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogProxy );
    }

    void KFLogProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLogProxy );
    }

    void KFLogProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
    }
}