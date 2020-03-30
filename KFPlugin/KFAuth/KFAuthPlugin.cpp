#include "KFAuthPlugin.hpp"
#include "KFAuthModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAuthPlugin::Install()
    {
        __REGISTER_MODULE__( KFAuth );
    }

    void KFAuthPlugin::UnInstall()
    {
        __UN_MODULE__( KFAuth );
    }

    void KFAuthPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_channel, KFChannelInterface );
        __FIND_MODULE__( _kf_account, KFAccountInterface );
        __FIND_MODULE__( _kf_dir_shard, KFDirShardInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

    void KFAuthPlugin::AddConfig()
    {
    }
}