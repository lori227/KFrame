#include "KFDebugPlugin.h"
#include "KFDebugModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDebugPlugin::Install()
    {
        __REGISTER_MODULE__( KFDebug );
    }

    void KFDebugPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDebug );
    }

    void KFDebugPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}