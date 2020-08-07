#include "KFTcpServerModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTcpServerModule::InitModule()
    {
        _server_engine = __NEW_OBJECT__( KFNetServerEngine );
    }

    KFTcpSetting* KFTcpServerModule::FindTcpServerSetting()
    {
        auto kfglobal = KFGlobal::Instance();

        auto kftcpsetting = KFTcpServerConfig::Instance()->FindTcpSetting( kfglobal->_app_name, kfglobal->_app_type );
        if ( kftcpsetting == nullptr )
        {
            kftcpsetting = KFTcpServerConfig::Instance()->FindTcpSetting( _globbing_string, kfglobal->_app_type );
            if ( kftcpsetting == nullptr )
            {
                kftcpsetting = KFTcpServerConfig::Instance()->FindTcpSetting( kfglobal->_app_name, _globbing_string );
                if ( kftcpsetting == nullptr )
                {
                    kftcpsetting = KFTcpServerConfig::Instance()->FindTcpSetting( _globbing_string, _globbing_string );
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

    void KFTcpServerModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ, &KFTcpServerModule::HandleRegisterReq );
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        auto kftcpsetting = FindTcpServerSetting();
        _compress_length = kftcpsetting->_compress_length;
        _is_open_encrypt = kftcpsetting->_open_encrypt;
        _server_engine->InitEngine( kftcpsetting->_max_queue_size,
                                    kftcpsetting->_handle_message_count,
                                    kftcpsetting->_message_type,
                                    KFTcpServerConfig::Instance()->_compress_type,
                                    KFTcpServerConfig::Instance()->_compress_level,
                                    kftcpsetting->_compress_length,
                                    KFTcpServerConfig::Instance()->_encrypt_key,
                                    kftcpsetting->_open_encrypt );

        _server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _server_engine->BindLostFunction( this, &KFTcpServerModule::OnServerLostHandle );

        // 启动tcp服务器
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_listen_port = kftcpsetting->_port;
        auto result = _server_engine->StartEngine( kftcpsetting->_local_ip, kftcpsetting->_port );
        if ( result == 0 )
        {
            __LOG_INFO__( "[{}:{}|{}:{}] tcp services ok",
                          kfglobal->_app_name, kfglobal->_app_type, kfglobal->_interanet_ip, kfglobal->_listen_port );
        }
        else
        {
            __LOG_ERROR__( "[{}:{}|{}:{}] tcp services failed[{}]",
                           kfglobal->_app_name, kfglobal->_app_type, kfglobal->_interanet_ip, kfglobal->_listen_port, result );
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
        if ( !netdata->_name.empty() )
        {
            __LOG_WARN__( "[{}:{}:{}] lost connect", netdata->_name, netdata->_type, netdata->_str_id );
        }
        else
        {
            __LOG_DEBUG__( "[{}:{}] lost connect", netdata->_session, netdata->_id );
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

    void KFTcpServerModule::AddTranspondFunction( KFModule* module, KFTranspondFunction& function )
    {
        _kf_transpond_function = function;
    }

    void KFTcpServerModule::RemoveTranspondFunction( KFModule* module )
    {
        _kf_transpond_function = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        bool result = __CALL_MESSAGE__( route, msgid, data, length );
        if ( result )
        {
            return;
        }

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( route, msgid, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp server transpond msgid[{}] failed", msgid );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find function", msgid );
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

        // handle 数据
        auto netdata = &kfhandle->_net_data;
        netdata->_session = handlid;
        netdata->_name = listendata->appname();
        netdata->_type = listendata->apptype();
        netdata->_id = listendata->appid();
        netdata->_ip = listendata->ip();
        netdata->_port = listendata->port();
        netdata->_str_id = KFAppId::ToString( listendata->appid() );

        // 注册回馈
        auto kfglobal = KFGlobal::Instance();
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( kfglobal->_app_type );
        ack.set_appname( kfglobal->_app_name );
        ack.set_appid( kfglobal->_app_id->GetId() );
        ack.set_compresslength( _compress_length );
        ack.set_compresstype( KFTcpServerConfig::Instance()->_compress_type );
        ack.set_compresslevel( KFTcpServerConfig::Instance()->_compress_level );
        ack.set_openencrypt( _is_open_encrypt );
        ack.set_encryptkey( KFTcpServerConfig::Instance()->_encrypt_key );
        SendNetMessage( handlid, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 初始化压缩加密
        kfhandle->InitCompressEncrypt();
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 注册新客户端回调
        CallDiscoverFunction( kfhandle );
        __LOG_INFO__( "[{}:{}:{}|{}:{}] register ok", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint64 excludeid /* = 0 */ )
    {
        auto strdata = message->SerializeAsString();
        SendNetMessage( msgid, strdata.data(), strdata.size(), excludeid );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint64 excludeid /* = 0 */ )
    {
        _server_engine->SendNetMessage( msgid, data, length, excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint32 msgid, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handleid, msgid, strdata.data(), strdata.size(), delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint32 msgid, const char* data, uint32 length, uint32 delay )
    {
        return _server_engine->SendNetMessage( handleid, msgid, data, length, delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay )
    {
        return _server_engine->SendNetMessage( handleid, recvid, msgid, data, length, delay );
    }

    bool KFTcpServerModule::SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, google::protobuf::Message* message, uint32 delay )
    {
        auto strdata = message->SerializeAsString();
        return _server_engine->SendNetMessage( handleid, recvid, msgid, strdata.data(), strdata.size(), delay );
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