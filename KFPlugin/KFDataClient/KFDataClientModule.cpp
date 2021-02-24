#include "KFDataClientModule.hpp"

namespace KFrame
{
#define __KEEPER_SAVE_TIMER__ 10000

    void KFDataClientModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleSavePlayerToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOAD_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleLoadPlayerToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleQueryPlayerToGameAck );
    }

    void KFDataClientModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_LOAD_PLAYER_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_TO_GAME_ACK );
    }

    void KFDataClientModule::Run()
    {
        auto now_time = KFGlobal::Instance()->_game_time;
        for ( auto& iter : _data_keeper._objects )
        {
            auto keeper = iter.second;
            if ( keeper->_save_time <= now_time )
            {
                keeper->_save_time = now_time + __KEEPER_SAVE_TIMER__;
                SaveKeeperData( keeper->_zone_id, keeper->_player_id, &keeper->_pb_object, keeper->_save_flag );
            }
        }

        for ( auto& iter : _load_keeper._objects )
        {
            auto keeper = iter.second;
            if ( keeper->_load_time <= now_time )
            {
                keeper->_load_time = now_time + __KEEPER_SAVE_TIMER__;
                LoadKeeperData( &keeper->_pb_login );
            }
        }
    }

    void KFDataClientModule::SetLoadPlayerFunction( KFLoadPlayerFunction& function )
    {
        _load_player_function = function;
    }

    void KFDataClientModule::SetQueryPlayerFunction( KFQueryPlayerFunction& function )
    {
        _query_player_function = function;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFDataClientModule::CalcZoneId( uint64 player_id )
    {
        return KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), player_id );
    }

    bool KFDataClientModule::LoadPlayerData( const KFMsg::PBLoginData* pblogin )
    {
        // 判断是否在缓存中
        {
            auto keeper = _data_keeper.Find( pblogin->player_id() );
            if ( keeper != nullptr )
            {
                __LOG_INFO__( "player=[{}] keeper load", pblogin->player_id() );
                _load_player_function( KFMsg::Ok, pblogin, &keeper->_pb_object );
                return true;
            }
        }

        // 创建load keeper
        auto keeper = _load_keeper.Create( pblogin->player_id() );
        keeper->_pb_login.CopyFrom( *pblogin );
        keeper->_pb_login.set_zoneid( CalcZoneId( pblogin->player_id() ) );
        keeper->_load_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;

        LoadKeeperData( &keeper->_pb_login );
        return true;
    }

    void KFDataClientModule::LoadKeeperData( const KFMsg::PBLoginData* pblogin )
    {
        __LOG_INFO__( "load palyer=[{}] req", pblogin->player_id() );

        // 加载玩家数据
        KFMsg::S2SLoadPlayerToDataReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = _kf_route->SendToRand( pblogin->player_id(), __STRING__( data ), KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "load palyer=[{}] failed", pblogin->player_id() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleLoadPlayerToGameAck, KFMsg::S2SLoadPlayerToGameAck )
    {
        auto pblogin = &kfmsg->pblogin();
        auto keeper = _load_keeper.Find( pblogin->player_id() );
        if ( keeper == nullptr || keeper->_pb_login.sessionid() != pblogin->sessionid() )
        {
            return __LOG_ERROR__( "player=[{}] load session error", pblogin->player_id() );
        }

        _load_keeper.Remove( pblogin->player_id() );
        __LOG_INFO__( "load palyer=[{}] ok", pblogin->player_id() );

        // 回调函数
        _load_player_function( kfmsg->result(), &kfmsg->pblogin(), &kfmsg->playerdata() );
    }

    void KFDataClientModule::RemoveLoadData( uint64 player_id )
    {
        _load_keeper.Remove( player_id );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::SavePlayerData( uint64 player_id, const KFMsg::PBObject* pbplayerdata, uint32 saveflag )
    {
        auto zone_id = CalcZoneId( player_id );
        auto keeper = _data_keeper.Create( player_id );
        keeper->_zone_id = zone_id;
        keeper->_player_id = player_id;
        keeper->_save_flag = saveflag;
        keeper->_pb_object.CopyFrom( *pbplayerdata );
        keeper->_save_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;

        SaveKeeperData( zone_id, player_id, pbplayerdata, saveflag );
        return true;
    }

    void KFDataClientModule::SaveKeeperData( uint32 zone_id, uint64 player_id, const KFMsg::PBObject* pbplayerdata, uint32 saveflag )
    {
        __LOG_INFO__( "save palyer=[{}] req", player_id );

        KFMsg::S2SSavePlayerToDataReq req;
        req.set_id( player_id );
        req.set_zoneid( zone_id );
        req.set_flag( saveflag );
        req.mutable_data()->CopyFrom( *pbplayerdata );
        auto ok = _kf_route->SendToRand( player_id, __STRING__( data ), KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "save palyer=[{}] failed", player_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleSavePlayerToGameAck, KFMsg::S2SSavePlayerToGameAck )
    {
        _data_keeper.Remove( kfmsg->id() );
        __LOG_INFO__( "save palyer=[{}] ok", kfmsg->id() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::QueryPlayerData( uint64 sendid, uint64 player_id )
    {
        auto keeper = _data_keeper.Find( player_id );
        if ( keeper != nullptr )
        {
            _query_player_function( KFMsg::Ok, sendid, &keeper->_pb_object );
            return true;
        }

        KFMsg::S2SQueryPlayerToDataReq req;
        req.set_playerid( player_id );
        req.set_zoneid( CalcZoneId( player_id ) );
        return _kf_route->SendToRand( sendid, __STRING__( data ), KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleQueryPlayerToGameAck, KFMsg::S2SQueryPlayerToGameAck )
    {
        auto player_id = __ROUTE_RECV_ID__;
        _query_player_function( kfmsg->result(), player_id, &kfmsg->playerdata() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
