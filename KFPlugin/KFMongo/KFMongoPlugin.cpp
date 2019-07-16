#include "KFMongoPlugin.hpp"
#include "KFMongoModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMongoPlugin::Install()
    {
        __REGISTER_MODULE__( KFMongo );
    }

    void KFMongoPlugin::UnInstall()
    {
        __UN_MODULE__( KFMongo );
    }

    void KFMongoPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}