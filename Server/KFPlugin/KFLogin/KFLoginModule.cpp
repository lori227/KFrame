#include "KFLoginModule.h"

namespace KFrame
{
    KFLoginModule::KFLoginModule()
    {
        _world_server_id = 0;
    }

    KFLoginModule::~KFLoginModule()
    {

    }

    void KFLoginModule::InitModule()
    {

    }

    void KFLoginModule::BeforeRun()
    {
        _kf_tcp_client->RegisterLostFunction( this, &KFLoginModule::OnClientLostWorld );
        _kf_tcp_client->RegisterConnectionFunction( this, &KFLoginModule::OnClientConnectionWorld );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFLoginModule::BeforeShut()
    {
        _kf_tcp_client->UnRegisterLostFunction( this );
        _kf_tcp_client->UnRegisterConnectionFunction( this );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFLoginModule::IsWorldConnected()
    {
        return _world_server_id != 0;
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFLoginModule::OnClientConnectionWorld )
    {
        if ( servertype != KFField::_world )
        {
            return;
        }

        _world_server_id = serverid;
    }

    __KF_CLIENT_LOST_FUNCTION__( KFLoginModule::OnClientLostWorld )
    {
        if ( servertype != KFField::_world )
        {
            return;
        }

        _world_server_id = 0;
    }

    bool KFLoginModule::SendMessageToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == 0 )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFLoginModule::SendMessageToGate( uint32 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }
}
