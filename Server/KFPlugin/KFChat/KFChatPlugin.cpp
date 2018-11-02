#include "KFChatPlugin.h"
#include "KFChatModule.h"

namespace KFrame
{
    void KFChatPlugin::Install()
    {
        __REGISTER_MODULE__( KFChat );
    }

    void KFChatPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFChat );
    }

    void KFChatPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_relation, KFRelationClientInterface );
    }

}