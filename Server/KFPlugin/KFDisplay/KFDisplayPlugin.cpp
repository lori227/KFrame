#include "KFDisplayPlugin.h"
#include "KFDisplayModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDisplayPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFDisplayPlugin, KFDisplayInterface >( new KFDisplayModule() );
    }

    void KFDisplayPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFDisplayPlugin, KFDisplayInterface >();
    }

    void KFDisplayPlugin::LoadModule()
    {
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();

        auto apptype = KFGlobal::Instance()->_app_type;
        if ( apptype == KFField::_shard )
        {
            _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
        }
        else if ( apptype == KFField::_game )
        {
            _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
            _kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
        }
    }
}