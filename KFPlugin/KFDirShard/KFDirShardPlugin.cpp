#include "KFDirShardPlugin.hpp"
#include "KFDirShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFDirShard );
    }

    void KFDirShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFDirShard );
    }

    void KFDirShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
        __FIND_MODULE__( _kf_dir_attribute, KFDirDatabaseInterface );
    }

    void KFDirShardPlugin::AddConfig()
    {
    }
}