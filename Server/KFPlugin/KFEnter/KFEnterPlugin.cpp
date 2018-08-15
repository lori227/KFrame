#include "KFEnterPlugin.h"
#include "KFEnterModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFEnterPlugin::Install()
    {
        __REGISTER_MODULE__( KFEnter );
    }

    void KFEnterPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFEnter );
    }

    void KFEnterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }
}