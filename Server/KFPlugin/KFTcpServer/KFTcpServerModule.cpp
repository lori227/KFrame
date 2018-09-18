#include "KFTcpServerModule.h"
#include "KFTcpServerConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFTcpServerModule::KFTcpServerModule()
    {
        _kf_server_engine = nullptr;
        _kf_transmit_function = nullptr;
    }

    KFTcpServerModule::~KFTcpServerModule()
    {
        __KF_DELETE__( _kf_server_engine );
    }

    void KFTcpServerModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_server_config, false );
        _kf_server_engine = __KF_NEW__( KFNetServerEngine );
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
                        return nullptr;
                    }
                }
            }
        }

        // 计算端口
        if ( kftcpsetting->_port == _invalid_int )
        {
            auto kfaddress = _kf_ip_address->FindIpAddress( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_str_app_id );
            kftcpsetting->_port_type = kfaddress->_port_type;
            kftcpsetting->_port = kfaddress->_port;
        }

        kftcpsetting->_port = _kf_ip_address->CalcListenPort( kftcpsetting->_port_type, kftcpsetting->_port, kfglobal->_app_id );
        return kftcpsetting;
    }

    void KFTcpServerModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ, &KFTcpServerModule::HandleRegisterReq );
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        auto kftcpsetting = FindTcpServerSetting();
        _kf_server_engine->InitEngine( kftcpsetting->_max_queue_size );
        _kf_server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _kf_server_engine->BindLostFunction( this, &KFTcpServerModule::OnServerLostHandle );

        // 启动tcp服务器
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_listen_port = kftcpsetting->_port;
        auto result = _kf_server_engine->StartEngine( kftcpsetting->_local_ip, kftcpsetting->_port, kftcpsetting->_max_connection, kftcpsetting->_time_out );
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
        __KF_REMOVE_CONFIG__();
        _kf_server_engine->ShutEngine();
    }

    void KFTcpServerModule::Run()
    {
        _kf_server_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFTcpServerModule::OnServerLostHandle )
    {
        auto kfhandle = _kf_server_engine->FindNetHandle( handleid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        BroadcastLostFromServer( kfhandle );
        CallLostFunction( kfhandle );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTcpServerModule::RegisteNetHandle( uint32 sessionid, uint32 handleid, uint32 objectid )
    {
        auto kfhandle = _kf_server_engine->RegisteHandle( sessionid, handleid, objectid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        return true;
    }

    bool KFTcpServerModule::CloseNetHandle( uint32 handleid, uint32 delaytime, const char* function, uint32 line )
    {
        return _kf_server_engine->CloseHandle( handleid, delaytime, function, line );
    }

    uint32 KFTcpServerModule::GetHandleCount()
    {
        return _kf_server_engine->GetHandleCount();
    }

    void KFTcpServerModule::GetHandleList( std::list<uint32>& handlelist )
    {
        _kf_server_engine->GetHandleList( handlelist );
    }

    const std::string& KFTcpServerModule::GetHandleIp( uint32 handleid )
    {
        return _kf_server_engine->GetHandleIp( handleid );
    }

    bool KFTcpServerModule::BindObjectId( uint32 handleid, uint32 objectid )
    {
        return _kf_server_engine->BindObjectId( handleid, objectid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::AddDiscoverFunction( const char* module, KFServerDiscoverFunction& function )
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
            kffunction->_function( tcphandle->_object_id, tcphandle->_app_name, tcphandle->_app_type, tcphandle->_listen_ip, tcphandle->_listen_port );
        }
    }

    void KFTcpServerModule::AddLostFunction( const char* module, KFServerLostFunction& function )
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
            kffunction->_function( tcphandle->_object_id, tcphandle->_app_name, tcphandle->_app_type );
        }
    }

    void KFTcpServerModule::AddTransmitFunction( const char* module, KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    void KFTcpServerModule::RemoveTransmitFunction( const char* module )
    {
        _kf_transmit_function = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        bool result = _kf_message->CallFunction( kfguid, msgid, data, length );
        if ( result )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( kfguid, msgid, data, length );
            if ( !ok )
            {
                __LOG_ERROR__( "tcp server transmit msgid[{}] failed!", msgid );
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
        __PROTO_PARSE__( KFMsg::RegisterToServerReq );
        auto sessionid = __KF_HEAD_ID__( kfguid );

        auto listendata = &kfmsg.listen();
        auto handlid = listendata->appid();
        auto name = listendata->appname();
        auto type = listendata->apptype();

        auto kfhandle = _kf_server_engine->RegisteHandle( sessionid, handlid, handlid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        kfhandle->_app_name = name;
        kfhandle->_app_type = type;
        kfhandle->_app_id = listendata->appid();
        kfhandle->_listen_ip = listendata->ip();
        kfhandle->_listen_port = listendata->port();
        kfhandle->_zone_id = listendata->zoneid();

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto kfglobal = KFGlobal::Instance();

        // 注册回馈
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( kfglobal->_app_type );
        ack.set_appid( kfglobal->_app_id );
        ack.set_appname( kfglobal->_app_name );
        SendNetMessage( handlid, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 广播新连接给所有连接着
        BroadcastRegisterToServer( kfhandle );

        // 注册信息发送个给新连接
        SendRegisterToServerToHandle( kfhandle );

        CallDiscoverFunction( kfhandle );

        __LOG_INFO__( "[{}:{}:{}|{}:{}] register ok!",
                      name, type, KFAppID::ToString( handlid ), listendata->ip(), listendata->port() );
    }

    void KFTcpServerModule::BroadcastRegisterToServer( KFNetHandle* kfhandle )
    {
        KFMsg::TellRegisterToServer tell;
        tell.set_servername( KFGlobal::Instance()->_app_name );
        tell.set_serverzoneid( KFGlobal::Instance()->_zone_id );

        auto listendata = tell.mutable_listen();
        listendata->set_appid( kfhandle->_app_id );
        listendata->set_appname( kfhandle->_app_name );
        listendata->set_apptype( kfhandle->_app_type );
        listendata->set_zoneid( kfhandle->_zone_id );
        listendata->set_ip( kfhandle->_listen_ip );
        listendata->set_port( kfhandle->_listen_port );
        SendNetMessage( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell, kfhandle->_object_id );
    }

    void KFTcpServerModule::SendRegisterToServerToHandle( KFNetHandle* kfhandle )
    {
        for ( auto& iter : _kf_server_engine->_kf_handles._objects )
        {
            auto nethandle = iter.second;
            KFMsg::TellRegisterToServer tell;
            tell.set_servername( KFGlobal::Instance()->_app_name );
            tell.set_serverzoneid( KFGlobal::Instance()->_zone_id );

            auto listendata = tell.mutable_listen();
            listendata->set_appid( nethandle->_app_id );
            listendata->set_appname( nethandle->_app_name );
            listendata->set_apptype( nethandle->_app_type );
            listendata->set_zoneid( nethandle->_zone_id );
            listendata->set_ip( nethandle->_listen_ip );
            listendata->set_port( nethandle->_listen_port );
            SendNetMessage( kfhandle->_object_id, KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
        }
    }

    void KFTcpServerModule::BroadcastLostFromServer( KFNetHandle* kfhandle )
    {
        if ( !kfhandle->IsServer() )
        {
            return;
        }

        KFMsg::TellUnRegisterFromServer tell;
        tell.set_appid( kfhandle->_app_id );
        tell.set_appname( kfhandle->_app_name );
        tell.set_apptype( kfhandle->_app_type );
        tell.set_zoneid( kfhandle->_zone_id );
        tell.set_servername( KFGlobal::Instance()->_app_name );
        tell.set_serverzoneid( KFGlobal::Instance()->_zone_id );
        SendNetMessage( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &tell, kfhandle->_object_id );

        __LOG_DEBUG__( "[{}:{}:{}|{}:{}] lost connect!",
                       kfhandle->_app_name, kfhandle->_app_type, KFAppID::ToString( kfhandle->_app_id ),
                       kfhandle->_listen_ip, kfhandle->_listen_port );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint32 excludeid /* = 0 */ )
    {
        auto strdata = message->SerializeAsString();
        SendNetMessage( msgid, strdata.data(), strdata.size(), excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_server_engine->SendNetMessage( handleid, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid /* = 0 */ )
    {
        _kf_server_engine->SendNetMessage( msgid, data, length, excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_server_engine->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_server_engine->SendNetMessage( handleid, objectid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_server_engine->SendNetMessage( handleid, objectid, msgid, strdata.data(), strdata.size() );
    }

    bool KFTcpServerModule::SendNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        auto handleid = __KF_HEAD_ID__( kfguid );
        auto objectid = __KF_DATA_ID__( kfguid );
        return SendNetMessage( handleid, objectid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( const KFGuid& kfguid, uint32 msgid, google::protobuf::Message* message )
    {
        auto handleid = __KF_HEAD_ID__( kfguid );
        auto objectid = __KF_DATA_ID__( kfguid );
        return SendNetMessage( handleid, objectid, msgid, message );
    }

    void KFTcpServerModule::SendMessageToName( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _kf_server_engine->SendMessageToName( name, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendMessageToType( const std::string& type, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _kf_server_engine->SendMessageToType( type, msgid, strdata.data(), strdata.size() );
    }
}