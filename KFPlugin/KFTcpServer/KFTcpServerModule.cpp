#include "KFTcpServerModule.hpp"
#include "KFTcpServerConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFTcpServerModule::KFTcpServerModule()
    {
        _server_engine = nullptr;
        _kf_transpond_function = nullptr;
    }

    KFTcpServerModule::~KFTcpServerModule()
    {
        delete _server_engine;
    }

    void KFTcpServerModule::InitModule()
    {
        _server_engine = new KFNetServerEngine();
        __KF_ADD_CONFIG__( _kf_server_config, false );
    }

    KFTcpSetting* KFTcpServerModule::FindTcpServerSetting()
    {
        auto kfglobal = KFGlobal::Instance();

        auto kftcpsetting = _kf_server_config->FindTcpSetting( kfglobal->_app_name, kfglobal->_app_type );
        if ( kftcpsetting == nullptr )
        {
            kftcpsetting = _kf_server_config->FindTcpSetting( _globbing_str, kfglobal->_app_type );
            if ( kftcpsetting == nullptr )
            {
                kftcpsetting = _kf_server_config->FindTcpSetting( kfglobal->_app_name, _globbing_str );
                if ( kftcpsetting == nullptr )
                {
                    kftcpsetting = _kf_server_config->FindTcpSetting( _globbing_str, _globbing_str );
                    if ( kftcpsetting == nullptr )
                    {
                        __LOG_ERROR__( "can't find [{}:{}] setting", kfglobal->_app_name, kfglobal->_app_type );
                        return nullptr;
                    }
                }
            }
        }

        // 计算端口
        if ( kftcpsetting->_port == _invalid_int )
        {
            auto kfaddress = _kf_ip_address->FindIpAddress( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
            if ( kfaddress == nullptr )
            {
                __LOG_ERROR__( "can't find [{}:{}:{}] ipaddress", kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
                return nullptr;
            }

            kftcpsetting->_port = kfaddress->_port;
            kftcpsetting->_port_type = kfaddress->_port_type;
        }

        kftcpsetting->_port = _kf_ip_address->CalcListenPort( kftcpsetting->_port_type, kftcpsetting->_port, kfglobal->_app_id->GetId() );
        return kftcpsetting;
    }

    void KFTcpServerModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ, &KFTcpServerModule::HandleRegisterReq );
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        auto kftcpsetting = FindTcpServerSetting();
        _server_engine->InitEngine( kftcpsetting->_max_queue_size, kftcpsetting->_message_type );
        _server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _server_engine->BindLostFunction( this, &KFTcpServerModule::OnServerLostHandle );

        // 启动tcp服务器
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_listen_port = kftcpsetting->_port;
        auto result = _server_engine->StartEngine( kftcpsetting->_local_ip, kftcpsetting->_port );
        if ( result == 0 )
        {
            __LOG_INFO__( "[{}:{}|{}:{}] tcp services ok!",
                          kfglobal->_app_name, kfglobal->_app_type, kfglobal->_interanet_ip, kfglobal->_listen_port );
        }
        else
        {
            __LOG_ERROR__( "[{}:{}|{}:{}] tcp services failed[{}]!",
                           kfglobal->_app_name, kfglobal->_app_type, kfglobal->_interanet_ip, kfglobal->_listen_port, result );
        }
    }

    void KFTcpServerModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ );
    }

    void KFTcpServerModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_server_config );
        _server_engine->ShutEngine();
    }

    void KFTcpServerModule::Run()
    {
        _server_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFTcpServerModule::OnServerLostHandle )
    {
        if ( !netdata->_name.empty() )
        {
            __LOG_ERROR__( "[{}:{}:{}] lost connect!", netdata->_name, netdata->_type, netdata->_str_id );
        }
        else
        {
            __LOG_DEBUG__( "[{}:{}] lost connect!", netdata->_session, netdata->_id );
        }

        auto kfhandle = _server_engine->FindNetHandle( netdata->_session );
        if ( kfhandle == nullptr )
        {
            return;
        }

        CallLostFunction( kfhandle );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTcpServerModule::RegisteNetHandle( uint64 sessionid, uint64 handleid, uint64 objectid )
    {
        auto kfhandle = _server_engine->RegisteHandle( sessionid, handleid, objectid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        return true;
    }

    bool KFTcpServerModule::CloseNetHandle( uint64 handleid, uint32 delaytime, const char* function, uint32 line )
    {
        return _server_engine->CloseHandle( handleid, delaytime, function, line );
    }

    bool KFTcpServerModule::HaveHandle( uint64 handleid )
    {
        auto kfhandle = _server_engine->FindNetHandle( handleid );
        return kfhandle != nullptr;
    }

    uint32 KFTcpServerModule::GetHandleCount()
    {
        return _server_engine->GetHandleCount();
    }

    void KFTcpServerModule::GetHandleList( NetDataList& outlist )
    {
        _server_engine->GetHandleList( outlist );
    }

    const std::string& KFTcpServerModule::GetHandleIp( uint64 handleid )
    {
        return _server_engine->GetHandleIp( handleid );
    }

    bool KFTcpServerModule::BindObjectId( uint64 handleid, uint64 objectid )
    {
        return _server_engine->BindObjectId( handleid, objectid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::AddDiscoverFunction( const char* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_discover_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveDiscoverFunction( const char* module )
    {
        _kf_discover_function.Remove( module );
    }

    void KFTcpServerModule::CallDiscoverFunction( KFNetHandle* tcphandle )
    {
        for ( auto& iter : _kf_discover_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( &tcphandle->_net_data );
        }
    }

    void KFTcpServerModule::AddLostFunction( const char* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_lost_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveLostFunction( const char* module )
    {
        _kf_lost_function.Remove( module );
    }

    void KFTcpServerModule::CallLostFunction( KFNetHandle* tcphandle )
    {
        for ( auto& iter : _kf_lost_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( &tcphandle->_net_data );
        }
    }

    void KFTcpServerModule::AddTranspondFunction( const char* module, KFTranspondFunction& function )
    {
        _kf_transpond_function = function;
    }

    void KFTcpServerModule::RemoveTranspondFunction( const char* module )
    {
        _kf_transpond_function = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        bool result = _kf_message->CallFunction( route, msgid, data, length );
        if ( result )
        {
            return;
        }

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( route, msgid, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp server transpond msgid[{}] failed!", msgid );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find function!", msgid );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTcpServerModule::HandleRegisterReq )
    {
        auto sessionid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::RegisterToServerReq );

        auto listendata = &kfmsg.listen();
        auto handlid = listendata->appid();
        auto kfhandle = _server_engine->RegisteHandle( sessionid, handlid, handlid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        auto netdata = &kfhandle->_net_data;
        netdata->_name = listendata->appname();
        netdata->_type = listendata->apptype();
        netdata->_id = listendata->appid();
        netdata->_ip = listendata->ip();
        netdata->_port = listendata->port();
        netdata->_str_id = KFAppId::ToString( listendata->appid() );
        netdata->_session = handlid;
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto kfglobal = KFGlobal::Instance();

        // 注册回馈
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( kfglobal->_app_type );
        ack.set_appname( kfglobal->_app_name );
        ack.set_appid( kfglobal->_app_id->GetId() );
        auto strdata = ack.SerializeAsString();
        SendNetMessage( handlid, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 注册新客户端回调
        CallDiscoverFunction( kfhandle );

        __LOG_INFO__( "[{}:{}:{}|{}:{}] register ok!", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint64 excludeid /* = 0 */ )
    {
        auto strdata = message->SerializeAsString();
        SendNetMessage( msgid, strdata.data(), strdata.size(), excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handleid, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint64 excludeid /* = 0 */ )
    {
        _server_engine->SendNetMessage( msgid, data, length, excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint32 msgid, const char* data, uint32 length )
    {
        return _server_engine->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, const char* data, uint32 length )
    {
        return _server_engine->SendNetMessage( handleid, recvid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handleid, recvid, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToName( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _server_engine->SendMessageToName( name, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToType( const std::string& type, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _server_engine->SendMessageToType( type, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToType( const std::string& type, uint32 msgid, const char* data, uint32 length )
    {
        _server_engine->SendMessageToType( type, msgid, data, length );
    }
}