#include "KFMySQLPlugin.h"
#include "KFMySQLModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMySQLPlugin::Install()
    {
        __REGISTER_MODULE__( KFMySQL );
    }

    void KFMySQLPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMySQL );
    }

    void KFMySQLPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}