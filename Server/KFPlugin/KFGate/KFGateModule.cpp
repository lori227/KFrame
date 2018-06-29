#include "KFGateModule.h"

namespace KFrame
{
    KFGateModule::KFGateModule()
    {

    }

    KFGateModule::~KFGateModule()
    {

    }

    void KFGateModule::InitModule()
    {

    }

    void KFGateModule::BeforeRun()
    {
        _kf_tcp_client->RegisterLostFunction( this, &KFGateModule::OnClientLostLogin );
        _kf_tcp_client->RegisterConnectionFunction( this, &KFGateModule::OnClientConnectionLogin );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGateModule::BeforeShut()
    {
        _kf_tcp_client->UnRegisterLostFunction( this );
        _kf_tcp_client->UnRegisterConnectionFunction( this );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGateModule::IsLoginConnected()
    {
        return _kf_login_conhash.GetHashNodeCount() != 0;
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFGateModule::OnClientConnectionLogin )
    {
        if ( servertype == KFField::_login )
        {
            _kf_login_conhash.AddHashNode( servertype, serverid, 100 );
        }
        else if ( servertype == KFField::_game )
        {
            _kf_game_conhash.AddHashNode( servertype, serverid, 100 );
        }
    }

    __KF_CLIENT_LOST_FUNCTION__( KFGateModule::OnClientLostLogin )
    {
        if ( servertype == KFField::_login )
        {
            _kf_login_conhash.RemoveHashNode( serverid );
        }
        else if ( servertype == KFField::_game )
        {
            _kf_game_conhash.RemoveHashNode( serverid );
        }
    }

    bool KFGateModule::SendMessageToLogin( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_client->SendMessageToType( KFField::_login, msgid, message );
        return true;
    }

    bool KFGateModule::SendMessageToLogin( uint32 accountid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto loginserverid = _kf_login_conhash.FindHashNode( accountid );
        if ( loginserverid == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( loginserverid, msgid, message );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 发送消息到游戏服务器
    bool KFGateModule::SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message )
    {
        auto gameid = _kf_game_conhash.FindHashNode( KFGlobal::Instance()->_app_id );
        if ( gameid == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( gameid, msgid, message );
    }

    bool KFGateModule::SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_client->SendNetMessage( gameid, msgid, message );
    }

    bool KFGateModule::SendMessageToGame( uint32 gameid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_client->SendNetMessage( gameid, playerid, msgid, message );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 发送消息到客户端
    void KFGateModule::SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    bool KFGateModule::SendMessageToClient( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( playerid, msgid, message );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGateModule::AddConnection( uint32 connectid, uint32 playerid )
    {
        return _kf_tcp_server->RegisteNetHandle( connectid, playerid );
    }

    bool KFGateModule::RemoveConnection( uint32 playerid, uint32 delaytime, const char* function, uint32 line )
    {
        return _kf_tcp_server->CloseNetHandle( playerid, delaytime, function, line );
    }

    const std::string& KFGateModule::GetIp( uint32 connectid )
    {
        return _kf_tcp_server->GetHandleIp( connectid );
    }
}
