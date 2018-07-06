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
        __REGISTER_SERVER_LOST_FUNCTION__( &KFDeployServerModule::OnServerLostClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ, &KFDeployServerModule::HandleUpdateServerStatusReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ, &KFDeployServerModule::HandleGetAgentIpAddressReq );


    }


    void KFDeployServerModule::ShutDown()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ );
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
        if ( handlename == KFField::_deploy )
        {
            _agent_list.Remove( handleid );
            _mysql_driver->Delete( KFField::_deploy, __KF_STRING__( handleid ) );
        }
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
        kfagentdata->_name = kfmsg.name();
        kfagentdata->_type = kfmsg.type();
        kfagentdata->_port = kfmsg.port();
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
        _mysql_driver->Insert( KFField::_deploy, values );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleGetAgentIpAddressReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGetAgentIpAddressReq );

        auto clientid = __KF_HEAD_ID__( kfguid );
        for ( auto& iter : _agent_list._objects )
        {
            auto* kfagent = iter.second;
            if ( kfagent->_local_ip == kfmsg.localip() )
            {
                KFMsg::S2SGetAgentIpAddressAck ack;
                ack.set_appname( kfagent->_name );
                ack.set_apptype( kfagent->_type );
                ack.set_appid( kfagent->_agent_id );
                ack.set_ip( kfagent->_local_ip );
                ack.set_port( kfagent->_port );
                _kf_tcp_server->SendNetMessage( clientid, KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK, &ack );
                break;
            }
        }
    }
}