#include "KFEnterPlugin.hpp"
#include "KFEnterModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFEnterPlugin::Install()
    {
        __REGISTER_MODULE__( KFEnter );
    }

    void KFEnterPlugin::UnInstall()
    {
        __UN_MODULE__( KFEnter );
    }

    void KFEnterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }
}