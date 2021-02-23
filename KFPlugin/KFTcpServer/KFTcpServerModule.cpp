#include "KFTcpServerModule.hpp"

namespace KFrame
{
    void KFTcpServerModule::InitModule()
    {
        _server_engine = __NEW_OBJECT__( KFNetServerEngine );
    }

    std::shared_ptr<KFTcpSetting> KFTcpServerModule::FindTcpServerSetting()
    {
        auto global = KFGlobal::Instance();

        auto tcp_setting = FindTcpSetting( global->_app_name, global->_app_type );
        if ( tcp_setting == nullptr )
        {
            tcp_setting = FindTcpSetting( _globbing_string, global->_app_type );
            if ( tcp_setting == nullptr )
            {
                tcp_setting = FindTcpSetting( global->_app_name, _globbing_string );
                if ( tcp_setting == nullptr )
                {
                    tcp_setting = FindTcpSetting( _globbing_string, _globbing_string );
                    if ( tcp_setting == nullptr )
                    {
                        __LOG_ERROR__( "can't find [{}:{}] setting", global->_app_name, global->_app_type );
                        return nullptr;
                    }
                }
            }
        }

        tcp_setting->_port = _kf_ip_address->CalcListenPort( tcp_setting->_port_type, tcp_setting->_port, global->_app_id->GetId() );
        return tcp_setting;
    }

    std::shared_ptr<KFTcpSetting> KFTcpServerModule::FindTcpSetting( const std::string& app_name, const std::string& app_type )
    {
        for ( auto& iter : KFTcpServerConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_app_name == app_name &&
                    setting->_app_type == app_type )
            {
                return setting;
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
        _tcp_setting = FindTcpServerSetting();
        _server_engine->InitEngine( _tcp_setting->_max_queue_size,
                                    _tcp_setting->_handle_message_count,
                                    _tcp_setting->_message_type,
                                    _tcp_setting->_compress_type,
                                    _tcp_setting->_compress_level,
                                    _tcp_setting->_compress_length,
                                    _tcp_setting->_encrypt_key,
                                    _tcp_setting->_open_encrypt );

        _server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _server_engine->BindEventFunction( KFNetDefine::DisconnectEvent,  this, &KFTcpServerModule::OnServerLostHandle );

        // 启动tcp服务器
        auto global = KFGlobal::Instance();
        global->_listen_port = _tcp_setting->_port;
        auto result = _server_engine->StartEngine( _tcp_setting->_local_ip, _tcp_setting->_port );
        if ( result == 0 )
        {
            __LOG_INFO__( "[{}:{}|{}:{}] tcp services ok",
                          global->_app_name, global->_app_type, global->_internet_ip, global->_listen_port );
        }
        else
        {
            __LOG_ERROR__( "[{}:{}|{}:{}] tcp services failed[{}]",
                           global->_app_name, global->_app_type, global->_internet_ip, global->_listen_port, result );
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

        auto handle = _server_engine->FindNetHandle( net_data->_session );
        if ( handle != nullptr )
        {
            CallLostFunction( handle );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTcpServerModule::RegisteNetHandle( uint64 session_id, uint64 handle_id, uint64 object_id )
    {
        auto handle = _server_engine->RegisterHandle( session_id, handle_id, object_id );
        return handle != nullptr;
    }

    bool KFTcpServerModule::CloseNetHandle( uint64 handle_id, uint32 delay_time, const char* function, uint32 line )
    {
        return _server_engine->CloseHandle( handle_id, delay_time, function, line );
    }

    bool KFTcpServerModule::HaveHandle( uint64 handle_id )
    {
        auto handle = _server_engine->FindNetHandle( handle_id );
        return handle != nullptr;
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

    bool KFTcpServerModule::BindObjectId( uint64 handle_id, uint64 object_id )
    {
        return _server_engine->BindObjectId( handle_id, object_id );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::AddDiscoverFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto function_data = _kf_discover_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFTcpServerModule::RemoveDiscoverFunction( KFModule* module )
    {
        _kf_discover_function.Remove( module );
    }

    void KFTcpServerModule::CallDiscoverFunction( std::shared_ptr<KFNetHandle> handle )
    {
        for ( auto& iter : _kf_discover_function._objects )
        {
            auto function = iter.second;
            function->Call( &handle->_net_data );
        }
    }

    void KFTcpServerModule::AddLostFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto function_data = _kf_lost_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFTcpServerModule::RemoveLostFunction( KFModule* module )
    {
        _kf_lost_function.Remove( module );
    }

    void KFTcpServerModule::CallLostFunction( std::shared_ptr<KFNetHandle> handle )
    {
        for ( auto& iter : _kf_lost_function._objects )
        {
            auto function = iter.second;
            function->Call( &handle->_net_data );
        }
    }

    void KFTcpServerModule::AddForwardFunction( KFModule* module, KFForwardFunction& function )
    {
        _kf_forward_function.SetFunction( module, function );
    }

    void KFTcpServerModule::RemoveForwardFunction( KFModule* module )
    {
        _kf_forward_function.Reset();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        bool result = __HANDLE_MESSAGE__( route, msg_id, data, length );
        if ( !result )
        {
            auto ok = _kf_forward_function.CallEx<bool>( route, msg_id, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp server forward msg_id=[{}] failed", msg_id );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTcpServerModule::HandleRegisterReq, KFMsg::RegisterToServerReq )
    {
        auto session_id = __ROUTE_SERVER_ID__;

        auto listen_data = &kfmsg->listen();
        auto handle_id = listen_data->appid();
        auto net_handle = _server_engine->RegisterHandle( session_id, handle_id, handle_id );
        if ( net_handle == nullptr )
        {
            return;
        }

        // handle 数据
        auto net_data = &net_handle->_net_data;
        net_data->_session = handle_id;
        net_data->_name = listen_data->appname();
        net_data->_type = listen_data->apptype();
        net_data->_id = listen_data->appid();
        net_data->_ip = listen_data->ip();
        net_data->_port = listen_data->port();
        net_data->_str_id = KFAppId::ToString( listen_data->appid() );

        // 注册回馈
        auto global = KFGlobal::Instance();
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( global->_app_type );
        ack.set_appname( global->_app_name );
        ack.set_appid( global->_app_id->GetId() );
        ack.set_compresslength( _tcp_setting->_compress_length );
        ack.set_compresstype( _tcp_setting->_compress_type );
        ack.set_compresslevel( _tcp_setting->_compress_level );
        ack.set_openencrypt( _tcp_setting->_open_encrypt );
        ack.set_encryptkey( _tcp_setting->_encrypt_key );
        SendNetMessage( handle_id, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 初始化压缩加密
        net_handle->InitCompressEncrypt();
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 注册新客户端回调
        CallDiscoverFunction( net_handle );
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