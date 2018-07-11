#include "KFDeployClientPlugin.h"
#include "KFDeployClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_deploy_command, KFDeployCommandInterface );

        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != KFField::_auth &&
                kfglobal->_app_name != KFField::_platform )
        {
            __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        }
    }
}