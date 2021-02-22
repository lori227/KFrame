#include "KFTcpDiscoverModule.hpp"

namespace KFrame
{
    void KFTcpDiscoverModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_DISCOVER__( &KFTcpDiscoverModule::OnServerDiscoverClient );
        __REGISTER_TCP_SERVER_LOST__( &KFTcpDiscoverModule::OnServerLostClient );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFTcpDiscoverModule::OnClientConnectServer );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_DISCOVER_SERVER_TO_MASTER, &KFTcpDiscoverModule::HandleTellDiscoverServerToMaster );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_LOST_SERVER_TO_MASTER, &KFTcpDiscoverModule::HandleTellLostServerToMaster );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_REGISTER_SERVER_TO_MASTER, &KFTcpDiscoverModule::HandleTellRegisterServerToMaster );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_SERVER_LIST_TO_MASTER, &KFTcpDiscoverModule::HandleTellServerListToMaster );
    }

    void KFTcpDiscoverModule::ShutDown()
    {
        __UN_TIMER_0__();
        __UN_TCP_SERVER_DISCOVER__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_TELL_DISCOVER_SERVER_TO_MASTER );
        __UN_MESSAGE__( KFMsg::S2S_TELL_LOST_SERVER_TO_MASTER );
        __UN_MESSAGE__( KFMsg::S2S_TELL_REGISTER_SERVER_TO_MASTER );
        __UN_MESSAGE__( KFMsg::S2S_TELL_SERVER_LIST_TO_MASTER );
    }

    void KFTcpDiscoverModule::PrepareRun()
    {
        auto global = KFGlobal::Instance();
        if ( global->_app_type != __STRING__( master ) )
        {
            return;
        }

        // 为了防止单点, master自动连接相同类型的服务器, 不在bus表里配置
        __LOOP_TIMER_0__( 60000u, 5000u, &KFTcpDiscoverModule::OnTimerQueryMasterList );
    }

    __KF_TIMER_FUNCTION__( KFTcpDiscoverModule::OnTimerQueryMasterList )
    {
        // 查询master列表
        auto& iplist = _kf_ip_address->GetMasterList( KFGlobal::Instance()->_app_name, KFGlobal::Instance()->_app_id->GetZoneId() );
        for ( auto& ipaddress : iplist )
        {
            _kf_tcp_client->StartClient( ipaddress._name, ipaddress._type, ipaddress._id, ipaddress._ip, ipaddress._port );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFTcpDiscoverModule::OnServerDiscoverClient )
    {
        // 不是相同服务不要广播
        auto global = KFGlobal::Instance();
        if ( global->_app_name != net_data->_name )
        {
            return;
        }

        // 如果是master, 则连接( 为了做master多点 )
        if ( net_data->_type == global->_app_type )
        {
            _kf_tcp_client->StartClient( net_data );
        }
        else
        {
            KFMsg::ListenData listendata;
            listendata.set_appname( net_data->_name );
            listendata.set_apptype( net_data->_type );
            listendata.set_appid( net_data->_id );
            listendata.set_ip( net_data->_ip );
            listendata.set_port( net_data->_port );

            // 广播新连接给所有连接
            {
                KFMsg::TellRegisterToServer tell;
                tell.mutable_listen()->CopyFrom( listendata );
                _kf_tcp_server->SendNetMessage( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell, net_data->_id );
            }

            // 所有连接信息发送给新连接
            {
                NetDataList handlelist;
                _kf_tcp_server->GetHandleList( handlelist );

                for ( auto kfhandle : handlelist )
                {
                    if ( kfhandle->_id == net_data->_id )
                    {
                        continue;
                    }

                    KFMsg::TellRegisterToServer tell;
                    auto listen = tell.mutable_listen();
                    listen->set_appname( kfhandle->_name );
                    listen->set_apptype( kfhandle->_type );
                    listen->set_appid( kfhandle->_id );
                    listen->set_ip( kfhandle->_ip );
                    listen->set_port( kfhandle->_port );
                    _kf_tcp_server->SendNetMessage( net_data->_id, KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
                }
            }

            // 通知其他master, 有新的连接
            KFMsg::S2STellDiscoverServerToMaster tell;
            tell.set_serverid( global->_app_id->GetId() );
            tell.mutable_listen()->CopyFrom( listendata );
            _kf_tcp_client->SendMessageToType( __STRING__( master ), KFMsg::S2S_TELL_DISCOVER_SERVER_TO_MASTER, &tell );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTcpDiscoverModule::HandleTellDiscoverServerToMaster, KFMsg::S2STellDiscoverServerToMaster )
    {
        auto listendata = &kfmsg->listen();

        // 广播新连接给所有连接
        {
            KFMsg::TellRegisterToServer tell;
            tell.mutable_listen()->CopyFrom( *listendata );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
        }

        // 所有连接信息发送给新连接
        {
            NetDataList handlelist;
            _kf_tcp_server->GetHandleList( handlelist );

            for ( auto kfhandle : handlelist )
            {
                KFMsg::S2STellRegisterServerToMaster tell;
                tell.set_serverid( listendata->appid() );

                auto listen = tell.mutable_listen();
                listen->set_appname( kfhandle->_name );
                listen->set_apptype( kfhandle->_type );
                listen->set_appid( kfhandle->_id );
                listen->set_ip( kfhandle->_ip );
                listen->set_port( kfhandle->_port );
                _kf_tcp_client->SendNetMessage( kfmsg->server_id(), KFMsg::S2S_TELL_REGISTER_SERVER_TO_MASTER, &tell );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTcpDiscoverModule::HandleTellRegisterServerToMaster, KFMsg::S2STellRegisterServerToMaster )
    {
        KFMsg::TellRegisterToServer tell;
        tell.mutable_listen()->CopyFrom( kfmsg->listen() );
        _kf_tcp_server->SendNetMessage( kfmsg->server_id(), KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
    }

    __KF_NET_EVENT_FUNCTION__( KFTcpDiscoverModule::OnServerLostClient )
    {
        // 不是相同服务不要广播
        auto global = KFGlobal::Instance();
        if ( global->_app_name != net_data->_name )
        {
            return;
        }

        if ( net_data->_type == global->_app_type )
        {
            _kf_tcp_client->CloseClient( net_data->_id, __FUNC_LINE__ );
        }
        else
        {
            {
                KFMsg::TellUnRegisterFromServer tell;
                tell.set_appname( net_data->_name );
                tell.set_apptype( net_data->_type );
                tell.set_appid( net_data->_id );
                _kf_tcp_server->SendNetMessage( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &tell, net_data->_id );
            }

            {
                // 通知其他master
                KFMsg::S2STellLostServerToMaster tell;
                tell.set_appname( net_data->_name );
                tell.set_apptype( net_data->_type );
                tell.set_appid( net_data->_id );
                _kf_tcp_client->SendMessageToType( __STRING__( master ), KFMsg::S2S_TELL_LOST_SERVER_TO_MASTER, &tell );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTcpDiscoverModule::HandleTellLostServerToMaster, KFMsg::S2STellLostServerToMaster )
    {
        KFMsg::TellUnRegisterFromServer tell;
        tell.set_appname( kfmsg->appname() );
        tell.set_apptype( kfmsg->apptype() );
        tell.set_appid( kfmsg->appid() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &tell );
    }

    __KF_NET_EVENT_FUNCTION__( KFTcpDiscoverModule::OnClientConnectServer )
    {
        auto global = KFGlobal::Instance();
        if ( global->_app_name != net_data->_name && global->_app_type != net_data->_type )
        {
            return;
        }

        KFMsg::S2STellServerListToMaster tell;

        NetDataList handlelist;
        _kf_tcp_server->GetHandleList( handlelist );
        for ( auto kfhandle : handlelist )
        {
            auto listen = tell.add_listen();
            listen->set_appname( kfhandle->_name );
            listen->set_apptype( kfhandle->_type );
            listen->set_appid( kfhandle->_id );
            listen->set_ip( kfhandle->_ip );
            listen->set_port( kfhandle->_port );
        }

        _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_TELL_SERVER_LIST_TO_MASTER, &tell );
    }

    __KF_MESSAGE_FUNCTION__( KFTcpDiscoverModule::HandleTellServerListToMaster, KFMsg::S2STellServerListToMaster )
    {
        for ( auto i = 0; i < kfmsg->listen_size(); ++i )
        {
            KFMsg::TellRegisterToServer tell;
            tell.mutable_listen()->CopyFrom( kfmsg->listen( i ) );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
        }
    }
}