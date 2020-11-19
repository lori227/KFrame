#include "KFMongoPlugin.hpp"
#include "KFMongoModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
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
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFMongoPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFMongoConfig );
    }
}