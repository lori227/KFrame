#include "KFDataClientModule.hpp"

namespace KFrame
{
#define __KEEPER_SAVE_TIMER__ 10000

    void KFDataClientModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleSavePlayerToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOAD_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleLoadPlayerToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_TO_GAME_ACK, &KFDataClientModule::HandleQueryPlayerToGameAck );
    }

    void KFDataClientModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_LOAD_PLAYER_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_TO_GAME_ACK );
    }

    void KFDataClientModule::Run()
    {
        auto nowtime = KFGlobal::Instance()->_game_time;
        for ( auto& iter : _data_keeper._objects )
        {
            auto keeper = iter.second;
            if ( keeper->_save_time <= nowtime )
            {
                keeper->_save_time = nowtime + __KEEPER_SAVE_TIMER__;
                SaveKeeperData( keeper->_player_id, &keeper->_pb_object, keeper->_save_flag );
            }
        }

        for ( auto& iter : _load_keeper._objects )
        {
            auto keeper = iter.second;
            if ( keeper->_load_time <= nowtime )
            {
                keeper->_load_time = nowtime + __KEEPER_SAVE_TIMER__;
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
    bool KFDataClientModule::LoadPlayerData( const KFMsg::PBLoginData* pblogin )
    {
        // 判断是否在缓存中
        auto keeper = _data_keeper.Find( pblogin->playerid() );
        if ( keeper != nullptr )
        {
            __LOG_INFO__( "player=[{}] keeper load", pblogin->playerid() );
            _load_player_function( KFMsg::Ok, pblogin, &keeper->_pb_object );
            return true;
        }

        LoadKeeperData( pblogin );
        {
            auto keeper = _load_keeper.Create( pblogin->playerid() );
            keeper->_pb_login.CopyFrom( *pblogin );
            keeper->_load_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;
        }
        return true;
    }

    void KFDataClientModule::LoadKeeperData( const KFMsg::PBLoginData* pblogin )
    {
        __LOG_INFO__( "load palyer=[{}] req", pblogin->playerid() );

        // 加载玩家数据
        KFMsg::S2SLoadPlayerToDataReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = _kf_route->SendToRand( pblogin->playerid(), __STRING__( data ), KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "load palyer=[{}] failed", pblogin->playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleLoadPlayerToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoadPlayerToGameAck );

        auto pblogin = &kfmsg.pblogin();
        auto keeper = _load_keeper.Find( pblogin->playerid() );
        if ( keeper == nullptr || keeper->_pb_login.sessionid() != pblogin->sessionid() )
        {
            return __LOG_ERROR__( "player=[{}] load session error", pblogin->playerid() );
        }

        _load_keeper.Remove( pblogin->playerid() );
        __LOG_INFO__( "load palyer=[{}] ok", pblogin->playerid() );

        // 回调函数
        _load_player_function( kfmsg.result(), &kfmsg.pblogin(), &kfmsg.playerdata() );
    }

    void KFDataClientModule::RemoveLoadData( uint64 playerid )
    {
        _load_keeper.Remove( playerid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::SavePlayerData( uint64 playerid, const KFMsg::PBObject* pbplayerdata, uint32 saveflag )
    {
        SaveKeeperData( playerid, pbplayerdata, saveflag );

        auto keeper = _data_keeper.Create( playerid );
        keeper->_player_id = playerid;
        keeper->_save_flag = saveflag;
        keeper->_pb_object.CopyFrom( *pbplayerdata );
        keeper->_save_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;
        return true;
    }

    void KFDataClientModule::SaveKeeperData( uint64 playerid, const KFMsg::PBObject* pbplayerdata, uint32 saveflag )
    {
        __LOG_INFO__( "save palyer=[{}] req", playerid );

        KFMsg::S2SSavePlayerToDataReq req;
        req.set_id( playerid );
        req.set_flag( saveflag );
        req.mutable_data()->CopyFrom( *pbplayerdata );
        auto ok = _kf_route->SendToRand( playerid, __STRING__( data ), KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "save palyer=[{}] failed", playerid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleSavePlayerToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SSavePlayerToGameAck );

        _data_keeper.Remove( kfmsg.id() );

        __LOG_INFO__( "save palyer=[{}] ok", kfmsg.id() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::QueryPlayerData( uint64 sendid, uint64 playerid )
    {
        auto keeper = _data_keeper.Find( playerid );
        if ( keeper != nullptr )
        {
            _query_player_function( KFMsg::Ok, sendid, &keeper->_pb_object );
            return true;
        }

        KFMsg::S2SQueryPlayerToDataReq req;
        req.set_playerid( playerid );
        return _kf_route->SendToRand( sendid, __STRING__( data ), KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleQueryPlayerToGameAck )
    {
        auto playerid = __ROUTE_RECV_ID__;
        __PROTO_PARSE__( KFMsg::S2SQueryPlayerToGameAck );

        // 回调函数
        _query_player_function( kfmsg.result(), playerid, &kfmsg.playerdata() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
