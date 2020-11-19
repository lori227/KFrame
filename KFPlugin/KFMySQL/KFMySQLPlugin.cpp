#include "KFMySQLPlugin.hpp"
#include "KFMySQLModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMySQLPlugin::Install()
    {
        __REGISTER_MODULE__( KFMySQL );
    }

    void KFMySQLPlugin::UnInstall()
    {
        __UN_MODULE__( KFMySQL );
    }

    void KFMySQLPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFMySQLPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFMySQLConfig );
    }
}