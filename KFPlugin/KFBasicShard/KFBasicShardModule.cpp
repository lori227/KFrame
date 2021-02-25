#include "KFBasicShardModule.hpp"

namespace KFrame
{
    void KFBasicShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_UPDATE_INT_VALUE_TO_BASIC_REQ, &KFBasicShardModule::HandleUpdateIntValueToBasicReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_UPDATE_STR_VALUE_TO_BASIC_REQ, &KFBasicShardModule::HandleUpdateStrValueToBasicReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLEAR_ONLINE_TO_BASIC_REQ, &KFBasicShardModule::HandleClearOnlineToBasicReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_ATTRIBUTE_TO_BASIC_REQ, &KFBasicShardModule::HandleQueryAttributeToBasicReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_SET_PLAYER_NAME_TO_BASIC_REQ, &KFBasicShardModule::HandleSetPlayerNameToBasicReq );
    }

    void KFBasicShardModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_INT_VALUE_TO_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_STR_VALUE_TO_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLEAR_ONLINE_TO_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_ATTRIBUTE_TO_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_TO_BASIC_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFBasicShardModule::HandleUpdateIntValueToBasicReq, KFMsg::S2SUpdateIntValueToBasicReq )
    {
        StringUInt64 values;
        auto data = &kfmsg->pbdata();
        __PROTO_TO_MAP__( data, values );
        _kf_basic_database->UpdateBasicIntValue( __ROUTE_SEND_ID__, __ROUTE_SERVER_ID__, values );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicShardModule::HandleUpdateStrValueToBasicReq, KFMsg::S2SUpdateStrValueToBasicReq )
    {
        StringMap values;
        auto data = &kfmsg->pbdata();
        __PROTO_TO_MAP__( data, values );
        _kf_basic_database->UpdateBasicStrValue( __ROUTE_SEND_ID__, __ROUTE_SERVER_ID__, values );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicShardModule::HandleClearOnlineToBasicReq, KFMsg::S2SClearOnlineToBasicReq )
    {
        _kf_basic_database->ClearBasicServerId( kfmsg->serverid() );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicShardModule::HandleQueryAttributeToBasicReq, KFMsg::S2SQueryAttributeToBasicReq )
    {
        StringMap values;
        auto result = _kf_basic_database->QueryBasicAttribute( kfmsg->name(), kfmsg->zoneid(), values );

        KFMsg::S2SQueryAttributeToGameAck ack;
        ack.set_result( result );
        ack.set_name( kfmsg->name() );

        auto& data = *ack.mutable_pbdata();
        __MAP_TO_PROTO__( values, data );
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_ATTRIBUTE_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicShardModule::HandleSetPlayerNameToBasicReq, KFMsg::S2SSetPlayerNameToBasicReq )
    {
        auto result = _kf_basic_database->SetPlayerName( kfmsg->zoneid(), kfmsg->playerid(), kfmsg->oldname(), kfmsg->newname() );

        KFMsg::S2SSetPlayerNameToGameAck ack;
        ack.set_result( result );
        ack.set_name( kfmsg->newname() );
        ack.set_playerid( kfmsg->playerid() );
        ack.set_costdata( kfmsg->costdata() );
        _kf_route->SendToRoute( route, KFMsg::S2S_SET_PLAYER_NAME_TO_GAME_ACK, &ack );
    }
}