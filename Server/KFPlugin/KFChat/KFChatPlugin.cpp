#include "KFChatPlugin.h"
#include "KFChatModule.h"

namespace KFrame
{
    void KFChatPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFChatPlugin, KFChatInterface >( new KFChatModule() );
    }

    void KFChatPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFChatPlugin, KFChatInterface >();
    }

    void KFChatPlugin::LoadModule()
    {
        _kf_game = _kf_plugin_manage->FindModule< KFGameInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_relation = _kf_plugin_manage->FindModule< KFRelationClientInterface >();
        _kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
        _kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
    }

}