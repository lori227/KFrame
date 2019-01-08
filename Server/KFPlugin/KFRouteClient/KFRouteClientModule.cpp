#include "KFRouteClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteClientModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &KFRouteClientModule::HandleRouteMessageToClientAck );
    }

    void KFRouteClientModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::SetTransmitFunction( KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    void KFRouteClientModule::AddRouteConnectionFunction( const std::string& name, KFClusterConnectionFunction& function )
    {
        _kf_cluster->AddConnectionFunction( name, function );
    }

    void KFRouteClientModule::RemoveRouteConnectionFunction( const std::string& name )
    {
        _kf_cluster->RemoveConnectionFunction( name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToAll( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToAllReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_ALL_REQ, &req );
    }

    bool KFRouteClientModule::SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameAllReq req;
        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }

    bool KFRouteClientModule::SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameRandReq req;
        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }

    bool KFRouteClientModule::SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameBalanceReq req;
        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }

    bool KFRouteClientModule::SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameObjectReq req;
        req.set_name( name );
        req.set_objectid( objectid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }

    bool KFRouteClientModule::SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;
        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }

    bool KFRouteClientModule::SendToPlayer( uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToPlayerReq req;
        req.set_targetid( serverid );
        req.set_playerid( playerid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_sourceid( KFGlobal::Instance()->_app_id._union._id );
        return _kf_cluster->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &req );
    }


    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToClientAck );

        KFId kftempid;
        kftempid._head_id = kfmsg.sourceid();
        kftempid._data_id = kfmsg.playerid();

        auto msgdata = kfmsg.msgdata().data();
        auto msglength = static_cast< uint32 >( kfmsg.msgdata().length() );

        bool ok = _kf_message->CallFunction( kftempid, kfmsg.msgid(), msgdata, msglength );
        if ( ok )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( kftempid, kfmsg.msgid(), msgdata, msglength );
            if ( !ok )
            {
                __LOG_ERROR__( "route msgid[{}] failed!", kfmsg.msgid() );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find handle", kfmsg.msgid() );
        }
    }
}
