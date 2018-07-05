#include "KFDeployServerModule.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    KFDeployServerModule::KFDeployServerModule()
    {
        _mysql_driver = nullptr;
    }

    KFDeployServerModule::~KFDeployServerModule()
    {

    }

    void KFDeployServerModule::BeforeRun()
    {
        _kf_tcp_server->RegisterLostFunction( this, &KFDeployServerModule::OnServerLostClient );

        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ, &KFDeployServerModule::HandleUpdateServerStatusReq );
    }


    void KFDeployServerModule::ShutDown()
    {
        _kf_tcp_server->UnRegisterLostFunction( this );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ );
    }

    void KFDeployServerModule::OnceRun()
    {
        _mysql_driver = _kf_mysql->CreateExecute( KFField::_deploy );

        // 清空数据库中的数据
        _mysql_driver->Execute( "truncate table %s;", KFField::_deploy.c_str() );
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFDeployServerModule::OnServerLostClient )
    {
        _agent_list.Remove( handleid );
        _mysql_driver->Delete( KFField::_deploy, __KF_STRING__( handleid ) );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleRegisterAgentToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterAgentToServerReq );

        // 删除老数据
        _agent_list.Remove( kfmsg.agentid() );
        _mysql_driver->Delete( KFField::_deploy, __KF_STRING__( kfmsg.agentid() ) );

        auto kfagentdata = _agent_list.Create( kfmsg.agentid() );
        kfagentdata->_agent_id = kfmsg.agentid();
        kfagentdata->_local_ip = kfmsg.localip();
        kfagentdata->_interanet_ip = kfmsg.interanetip();
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleUpdateServerStatusReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateServerStatusReq );

        auto kfagentdata = _agent_list.Find( kfmsg.agentid() );
        if ( kfagentdata == nullptr )
        {
            return;
        }

        auto kfserverdata = kfagentdata->_server_list.Create( kfmsg.appid() );
        kfserverdata->_app_id = kfmsg.appid();
        kfserverdata->_app_name = kfmsg.appname();
        kfserverdata->_app_type = kfmsg.apptype();
        kfserverdata->_process_id = kfmsg.process();
        kfserverdata->_startup_time = kfmsg.startuptime();
        kfserverdata->_is_shutdown = kfmsg.isshutdown();

        // 更新数据库
        MapString values;
        values[ KFField::_app_id ] = __KF_STRING__( kfserverdata->_app_id );
        values[ KFField::_app_name ] = kfserverdata->_app_name;
        values[ KFField::_app_type ] = kfserverdata->_app_type;
        values[ KFField::_process ] = __KF_STRING__( kfserverdata->_process_id );
        values[ KFField::_time ] = __KF_STRING__( kfserverdata->_startup_time );
        values[ KFField::_shut_down ] = __KF_STRING__( kfserverdata->_is_shutdown ? 1 : 0 );
        values[ KFField::_agent_id ] = __KF_STRING__( kfagentdata->_agent_id );
        values[ KFField::_local_ip ] = kfagentdata->_local_ip;
        values[ KFField::_interanet_ip ] = kfagentdata->_interanet_ip;
        _mysql_driver->Insert( KFField::_deploy, values );
    }
}