#include "KFClusterProxyPlugin.hpp"
#include "KFClusterProxyModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterProxy );
    }

    void KFClusterProxyPlugin::UnInstall()
    {
        __UN_MODULE__( KFClusterProxy );
    }

    void KFClusterProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}