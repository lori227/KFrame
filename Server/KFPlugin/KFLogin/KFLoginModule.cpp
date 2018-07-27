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
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFLoginModule::OnClientLostWorld );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFLoginModule::OnClientConnectionWorld );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFLoginModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFLoginModule::IsWorldConnected()
    {
        return _world_server_id != 0;
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFLoginModule::OnClientConnectionWorld )
    {
        if ( servertype != __KF_STRING__( world ) )
        {
            return;
        }

        _world_server_id = serverid;
    }

    __KF_CLIENT_LOST_FUNCTION__( KFLoginModule::OnClientLostWorld )
    {
        if ( servertype != __KF_STRING__( world ) )
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
