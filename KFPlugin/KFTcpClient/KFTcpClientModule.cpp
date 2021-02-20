#include "KFTcpClientModule.hpp"
#include "KFNetwork/KFNetClient.hpp"

namespace KFrame
{
    void KFTcpClientModule::InitModule()
    {
        _client_engine = __NEW_OBJECT__( KFNetClientEngine );
    }

    void KFTcpClientModule::BeforeRun()
    {
        _client_engine->InitEngine( KFGlobal::Instance()->_game_time, 20000, KFMessageEnum::Server );

        _client_engine->BindNetFunction( this, &KFTcpClientModule::HandleNetMessage );
        _client_engine->BindConnectFunction( this, &KFTcpClientModule::OnClientConnected );
        _client_engine->BindDisconnectFunction( this, &KFTcpClientModule::OnClientDisconnect );
        _client_engine->BindShutdownFunction( this, &KFTcpClientModule::OnClientShutdown );
        _client_engine->BindFailedFunction( this, &KFTcpClientModule::OnClientFailed );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &KFTcpClientModule::HandleRegisterAck );
    }

    void KFTcpClientModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_ACK );
        ////////////////////////////////////////////////////////////////////////////
    }

    void KFTcpClientModule::ShutDown()
    {
        _client_engine->ShutEngine();
        __DELETE_OBJECT__( _client_engine );
    }

    void KFTcpClientModule::Run()
    {
        // 网络事件
        _client_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    // 连接成功
    __KF_NET_EVENT_FUNCTION__( KFTcpClientModule::OnClientConnected )
    {
        auto kfglobal = KFGlobal::Instance();

        // 把自己注册到服务器
        KFMsg::RegisterToServerReq req;
        auto listendata = req.mutable_listen();
        listendata->set_appname( kfglobal->_app_name );
        listendata->set_apptype( kfglobal->_app_type );
        listendata->set_appid( kfglobal->_app_id->GetId() );
        listendata->set_ip( kfglobal->_interanet_ip );
        listendata->set_port( kfglobal->_listen_port );
        SendNetMessage( netdata->_id, KFMsg::S2S_REGISTER_TO_SERVER_REQ, &req );
    }

    // 连接断开
    __KF_NET_EVENT_FUNCTION__( KFTcpClientModule::OnClientDisconnect )
    {
        CallClientLostFunction( netdata );
    }

    // 连接关闭
    __KF_NET_EVENT_FUNCTION__( KFTcpClientModule::OnClientShutdown )
    {
        CallClientShutdownFunction( netdata );
    }

    // 连接失败
    __KF_NET_EVENT_FUNCTION__( KFTcpClientModule::OnClientFailed )
    {
        CallClientFailedFunction( netdata );
    }

    void KFTcpClientModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        _client_engine->SendNetMessage( msgid, strdata.data(), strdata.size(), delay );
    }

    bool KFTcpClientModule::SendNetMessage( uint64 serverid, uint32 msgid, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _client_engine->SendNetMessage( serverid, msgid, strdata.data(), strdata.size(), delay );
    }

    bool KFTcpClientModule::SendNetMessage( uint64 serverid, uint64 recvid, uint32 msgid, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _client_engine->SendNetMessage( serverid, recvid, msgid, strdata.data(), strdata.size(), delay );
    }

    void KFTcpClientModule::SendNetMessage( uint32 msgid, const char* data, uint32 length )
    {
        _client_engine->SendNetMessage( msgid, data, length );
    }

    bool KFTcpClientModule::SendNetMessage( uint64 serverid, uint32 msgid, const char* data, uint32 length )
    {
        return _client_engine->SendNetMessage( serverid, msgid, data, length );
    }

    bool KFTcpClientModule::SendNetMessage( uint64 serverid, uint64 recvid, uint32 msgid, const char* data, uint32 length )
    {
        return _client_engine->SendNetMessage( serverid, recvid, msgid, data, length );
    }

    // 给某一类型服务器发送消息
    void KFTcpClientModule::SendMessageToType( const std::string& servertype, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToType( servertype, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToType( const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        _client_engine->SendMessageToType( servertype, msgid, data, length );
    }

    void KFTcpClientModule::SendMessageToName( const std::string& servername, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToName( servername, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToName( const std::string& servername, uint32 msgid, const char* data, uint32 length )
    {
        _client_engine->SendMessageToName( servername, msgid, data, length );
    }

    void KFTcpClientModule::SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToServer( servername, servertype, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        _client_engine->SendMessageToServer( servername, servertype, msgid, data, length );
    }

    void KFTcpClientModule::HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        bool handleresult = __HANDLE_MESSAGE__( route, msgid, data, length );
        if ( handleresult )
        {
            return;
        }

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( route, msgid, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp client transpond msgid[{}] failed", msgid );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find function", msgid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpClientModule::StartClient( const KFNetData* netdata )
    {
        if ( IsSelfConnection( netdata->_name, netdata->_type, netdata->_id ) )
        {
            return;
        }

        KFNetData kfnetdata;
        kfnetdata = *netdata;
        kfnetdata._str_id = KFAppId::ToString( kfnetdata._id );
        auto ok = _client_engine->StartClient( &kfnetdata );
        if ( ok )
        {
            __LOG_DEBUG__( "[{}:{}:{}|{}:{}] start connect",
                           kfnetdata._name, kfnetdata._type, kfnetdata._str_id, kfnetdata._ip, kfnetdata._port );
        }
    }

    void KFTcpClientModule::StartClient( const std::string& name, const std::string& type, uint64 id, const std::string& ip, uint32 port )
    {
        KFNetData netdata;
        netdata._name = name;
        netdata._type = type;
        netdata._id = id;
        netdata._ip = ip;
        netdata._port = port;
        StartClient( &netdata );
    }

    bool KFTcpClientModule::IsSelfConnection( const std::string& name, const std::string& type, uint64 id )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_id->GetId() == id )
        {
            return true;
        }

        return false;
    }

    void KFTcpClientModule::CloseClient( uint64 serverid, const char* function, uint32 line )
    {
        _client_engine->CloseClient( serverid, function, line );

        __LOG_DEBUG_FUNCTION__( function, line, "[{}] connect close", KFAppId::ToString( serverid ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpClientModule::AddConnectionFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_connection_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveConnectionFunction( KFModule* module )
    {
        _kf_connection_function.Remove( module );
    }

    void KFTcpClientModule::CallClientConnectionFunction( const KFNetData* netdata )
    {
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( netdata );
        }
    }

    void KFTcpClientModule::AddLostFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_lost_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveLostFunction( KFModule* module )
    {
        _kf_lost_function.Remove( module );
    }

    void KFTcpClientModule::CallClientLostFunction( const KFNetData* netdata )
    {
        for ( auto& iter : _kf_lost_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( netdata );
        }
    }

    void KFTcpClientModule::AddShutdownFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_shutdown_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveShutdownFunction( KFModule* module )
    {
        _kf_shutdown_function.Remove( module );
    }

    void KFTcpClientModule::CallClientShutdownFunction( const KFNetData* netdata )
    {
        for ( auto& iter : _kf_shutdown_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( netdata );
        }
    }

    void KFTcpClientModule::AddFailedFunction( KFModule* module, KFNetEventFunction& function )
    {
        auto kffunction = _kf_failed_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveFailedFunction( KFModule* module )
    {
        _kf_failed_function.Remove( module );
    }

    void KFTcpClientModule::CallClientFailedFunction( const KFNetData* netdata )
    {
        for ( auto& iter : _kf_failed_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( netdata );
        }
    }

    void KFTcpClientModule::RemoveTranspondFunction( KFModule* module )
    {
        _kf_transpond_function = nullptr;
    }

    void KFTcpClientModule::AddTranspondFunction( KFModule* module, KFForwardFunction& function )
    {
        _kf_transpond_function = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // 注册回馈
    __KF_MESSAGE_FUNCTION__( KFTcpClientModule::HandleRegisterAck, KFMsg::RegisterToServerAck )
    {
        auto kfclient = _client_engine->FindClient( __ROUTE_SERVER_ID__ );
        if ( kfclient != nullptr )
        {
            auto netdata = &kfclient->_net_data;
            __LOG_INFO__( "[{}:{}:{}|{}:{}] service ok", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );

            kfclient->InitCompressEncrypt( kfmsg->compresstype(), kfmsg->compresslevel(), kfmsg->compresslength(), kfmsg->encryptkey(), kfmsg->openencrypt() );

            // 处理回调函数
            CallClientConnectionFunction( netdata );
        }
        else
        {
            __LOG_ERROR__( "can't find client[{}|{}]", KFAppId::ToString( __ROUTE_SERVER_ID__ ), KFAppId::ToString( kfmsg->appid() ) );
        }
    }
}