#include "KFTcpServerModule.hpp"

namespace KFrame
{
    void KFTcpServerModule::InitModule()
    {
        _server_engine = __NEW_OBJECT__( KFNetServerEngine );
    }

    KFTcpSetting* KFTcpServerModule::FindTcpServerSetting()
    {
        auto kfglobal = KFGlobal::Instance();

        auto kftcpsetting = FindTcpSetting( kfglobal->_app_name, kfglobal->_app_type );
        if ( kftcpsetting == nullptr )
        {
            kftcpsetting = FindTcpSetting( _globbing_string, kfglobal->_app_type );
            if ( kftcpsetting == nullptr )
            {
                kftcpsetting = FindTcpSetting( kfglobal->_app_name, _globbing_string );
                if ( kftcpsetting == nullptr )
                {
                    kftcpsetting = FindTcpSetting( _globbing_string, _globbing_string );
                    if ( kftcpsetting == nullptr )
                    {
                        __LOG_ERROR__( "can't find [{}:{}] setting", kfglobal->_app_name, kfglobal->_app_type );
                        return nullptr;
                    }
                }
            }
        }

        kftcpsetting->_port = _kf_ip_address->CalcListenPort( kftcpsetting->_port_type, kftcpsetting->_port, kfglobal->_app_id->GetId() );
        return kftcpsetting;
    }

    KFTcpSetting* KFTcpServerModule::FindTcpSetting( const std::string& appname, const std::string& apptype )
    {
        for ( auto& iter : KFTcpServerConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_app_name == appname &&
                    kfsetting->_app_type == apptype )
            {
                return kfsetting;
            }
        }

        return nullptr;
    }


    void KFTcpServerModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_REGISTER_TO_SERVER_REQ, &KFTcpServerModule::HandleRegisterReq );
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        _kf_tcp_setting = FindTcpServerSetting();
        _server_engine->InitEngine( _kf_tcp_setting->_max_queue_size,
                                    _kf_tcp_setting->_handle_message_count,
                                    _kf_tcp_setting->_message_type,
                                    _kf_tcp_setting->_compress_type,
                                    _kf_tcp_setting->_compress_level,
                                    _kf_tcp_setting->_compress_length,
                                    _kf_tcp_setting->_encrypt_key,
                                    _kf_tcp_setting->_open_encrypt );

        _server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _server_engine->BindLostFunction( this, &KFTcpServerModule::OnServerLostHandle );

        // 启动tcp服务器
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_listen_port = _kf_tcp_setting->_port;
        auto result = _server_engine->StartEngine( _kf_tcp_setting->_local_ip, _kf_tcp_setting->_port );
        if ( result == 0 )
        {
            __LOG_INFO__( "[{}:{}|{}:{}] tcp services ok",
                          kfglobal->_app_name, kfglobal->_app_type, kfglobal->_intranet_ip, kfglobal->_listen_port );
        }
        else
        {
            __LOG_ERROR__( "[{}:{}|{}:{}] tcp services failed[{}]",
                           kfglobal->_app_name, kfglobal->_app_type, kfglobal->_intranet_ip, kfglobal->_listen_port, result );
        }
    }

    void KFTcpServerModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ );
    }

    void KFTcpServerModule::ShutDown()
    {
        _server_engine->ShutEngine();
        __DELETE_OBJECT__( _server_engine );
    }

    void KFTcpServerModule::Run()
    {
        _server_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFTcpServerModule::OnServerLostHandle )
    {
        if ( !net_data->_name.empty() )
        {
            __LOG_WARN__( "[{}:{}:{}] lost connect", net_data->_name, net_data->_type, net_data->_str_id );
        }
        else
        {
            __LOG_DEBUG__( "[{}:{}] lost connect", net_data->_session, net_data->_id );
        }

        auto kfhandle = _server_engine->FindNetHandle( net_data->_session );
        if ( kfhandle == nullptr )
        {
            return;
        }

        CallLostFunction( kfhandle );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTcpServerModule::RegisteNetHandle( uint64 sessionid, uint64 handle_id, uint64 objectid )
    {
        auto kfhandle = _server_engine->RegisterHandle( sessionid, handle_id, objectid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        return true;
    }

    bool KFTcpServerModule::CloseNetHandle( uint64 handle_id, uint32 delaytime, const char* function, uint32 line )
    {
        return _server_engine->CloseHandle( handle_id, delaytime, function, line );
    }

    bool KFTcpServerModule::HaveHandle( uint64 handle_id )
    {
        auto kfhandle = _server_engine->FindNetHandle( handle_id );
        return kfhandle != nullptr;
    }

    uint32 KFTcpServerModule::GetHandleCount()
    {
        return _server_engine->GetHandleCount();
    }

    void KFTcpServerModule::GetHandleList( NetDataList& out_list )
    {
        _server_engine->GetHandleList( out_list );
    }

    const std::string& KFTcpServerModule::GetHandleIp( uint64 handle_id )
    {
        return _server_engine->GetHandleIp( handle_id );
    }

    bool KFTcpServerModule::BindObjectId( uint64 handle_id, uint64 objectid )
    {
        return _server_engine->BindObjectId( handle_id, objectid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::AddDiscoverFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_discover_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveDiscoverFunction( KFModule* module )
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

    void KFTcpServerModule::AddLostFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_lost_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveLostFunction( KFModule* module )
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

    void KFTcpServerModule::AddTranspondFunction( KFModule* module, KFForwardFunction& function )
    {
        _kf_transpond_function = function;
    }

    void KFTcpServerModule::RemoveTranspondFunction( KFModule* module )
    {
        _kf_transpond_function = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        bool result = __HANDLE_MESSAGE__( route, msg_id, data, length );
        if ( result )
        {
            return;
        }

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( route, msg_id, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp server transpond msg_id[{}] failed", msg_id );
            }
        }
        else
        {
            __LOG_ERROR__( "msg_id[{}] can't find function", msg_id );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTcpServerModule::HandleRegisterReq, KFMsg::RegisterToServerReq )
    {
        auto sessionid = __ROUTE_SERVER_ID__;

        auto listendata = &kfmsg->listen();
        auto handlid = listendata->appid();
        auto kfhandle = _server_engine->RegisterHandle( sessionid, handlid, handlid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        // handle 数据
        auto net_data = &kfhandle->_net_data;
        net_data->_session = handlid;
        net_data->_name = listendata->appname();
        net_data->_type = listendata->apptype();
        net_data->_id = listendata->appid();
        net_data->_ip = listendata->ip();
        net_data->_port = listendata->port();
        net_data->_str_id = KFAppId::ToString( listendata->appid() );

        // 注册回馈
        auto kfglobal = KFGlobal::Instance();
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( kfglobal->_app_type );
        ack.set_appname( kfglobal->_app_name );
        ack.set_appid( kfglobal->_app_id->GetId() );
        ack.set_compresslength( _kf_tcp_setting->_compress_length );
        ack.set_compresstype( _kf_tcp_setting->_compress_type );
        ack.set_compresslevel( _kf_tcp_setting->_compress_level );
        ack.set_openencrypt( _kf_tcp_setting->_open_encrypt );
        ack.set_encryptkey( _kf_tcp_setting->_encrypt_key );
        SendNetMessage( handlid, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 初始化压缩加密
        kfhandle->InitCompressEncrypt();
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 注册新客户端回调
        CallDiscoverFunction( kfhandle );
        __LOG_INFO__( "[{}:{}:{}|{}:{}] register ok", net_data->_name, net_data->_type, net_data->_str_id, net_data->_ip, net_data->_port );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msg_id, google::protobuf::Message* message, uint64 exclude_id /* = 0 */ )
    {
        auto strdata = message->SerializeAsString();
        SendNetMessage( msg_id, strdata.data(), strdata.size(), exclude_id );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint64 exclude_id /* = 0 */ )
    {
        _server_engine->SendNetMessage( msg_id, data, length, exclude_id );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handle_id, uint32 msg_id, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handle_id, msg_id, strdata.data(), strdata.size(), delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handle_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        return _server_engine->SendNetMessage( handle_id, msg_id, data, length, delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        return _server_engine->SendNetMessage( handle_id, recv_id, msg_id, data, length, delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handle_id, recv_id, msg_id, strdata.data(), strdata.size(), delay );
    }

    void KFTcpServerModule::SendMessageToName( const std::string& name, uint32 msg_id, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _server_engine->SendMessageToName( name, msg_id, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToType( const std::string& type, uint32 msg_id, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _server_engine->SendMessageToType( type, msg_id, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToType( const std::string& type, uint32 msg_id, const char* data, uint32 length )
    {
        _server_engine->SendMessageToType( type, msg_id, data, length );
    }
}