#include "KFDataClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDataClientModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &KFDataClientModule::HandleLoadPlayerAck );
    }

    void KFDataClientModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK );
    }

    void KFDataClientModule::SetLoadPlayerFunction( KFLoadPlayerFunction& function )
    {
        _load_player_function = function;
    }

    bool KFDataClientModule::LoadPlayerData( const KFMsg::PBLoginData* pblogin )
    {
        //auto zoneid = KFUtility::CalcZoneId( pblogin->playerid() );

        // 加载玩家数据
        KFMsg::S2SLoginLoadPlayerReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        return _kf_route->SendToRand( __KF_STRING__( data ), KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleLoadPlayerAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerAck );

        // 回调函数
        _load_player_function( kfmsg.result(), &kfmsg.pblogin(), kfmsg.mutable_playerdata() );
    }

    bool KFDataClientModule::SavePlayerData( uint64 playerid, const KFMsg::PBObject* pbplayerdata )
    {
        KFMsg::S2SSavePlayerReq req;
        req.set_id( playerid );
        req.mutable_data()->CopyFrom( *pbplayerdata );
        return _kf_route->SendToRand( __KF_STRING__( data ), KFMsg::S2S_SAVE_PLAYER_REQ, &req );
    }
}
