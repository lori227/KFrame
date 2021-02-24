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

    void KFDataClientModule::SetLoadPlayerFunction( KFModule* module, KFLoadPlayerFunction& function )
    {
        _load_player_function.SetFunction( module, function );
    }

    void KFDataClientModule::SetQueryPlayerFunction( KFModule* module, KFQueryPlayerFunction& function )
    {
        _query_player_function.SetFunction( module, function );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFDataClientModule::CalcZoneId( uint64 player_id )
    {
        return KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), player_id );
    }

    bool KFDataClientModule::LoadPlayerData( const KFMsg::PBLoginData* login_data )
    {
        // 判断是否在缓存中
        {
            auto keeper = _data_keeper.Find( login_data->playerid() );
            if ( keeper != nullptr )
            {
                __LOG_INFO__( "player=[{}] keeper load", login_data->playerid() );
                _load_player_function.Call( KFMsg::Ok, login_data, &keeper->_pb_object );
                return true;
            }
        }

        // 创建load keeper
        auto keeper = _load_keeper.Create( login_data->playerid() );
        keeper->_pb_login.CopyFrom( *login_data );
        keeper->_pb_login.set_zoneid( CalcZoneId( login_data->playerid() ) );
        keeper->_load_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;

        LoadKeeperData( &keeper->_pb_login );
        return true;
    }

    void KFDataClientModule::LoadKeeperData( const KFMsg::PBLoginData* login_data )
    {
        __LOG_INFO__( "load player=[{}] req", login_data->playerid() );

        // 加载玩家数据
        KFMsg::S2SLoadPlayerToDataReq req;
        req.mutable_pblogin()->CopyFrom( *login_data );
        auto ok = _kf_route->SendToRand( login_data->playerid(), __STRING__( data ), KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "load player=[{}] failed", login_data->playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleLoadPlayerToGameAck, KFMsg::S2SLoadPlayerToGameAck )
    {
        auto login_data = &kfmsg->pblogin();
        auto keeper = _load_keeper.Find( login_data->playerid() );
        if ( keeper == nullptr || keeper->_pb_login.sessionid() != login_data->sessionid() )
        {
            return __LOG_ERROR__( "player=[{}] load session error", login_data->playerid() );
        }

        _load_keeper.Remove( login_data->playerid() );
        __LOG_INFO__( "load player=[{}] ok", login_data->playerid() );

        // 回调函数
        _load_player_function.Call( kfmsg->result(), &kfmsg->pblogin(), &kfmsg->playerdata() );
    }

    void KFDataClientModule::RemoveLoadData( uint64 player_id )
    {
        _load_keeper.Remove( player_id );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::SavePlayerData( uint64 player_id, const KFMsg::PBObject* player_data, uint32 save_flag )
    {
        auto zone_id = CalcZoneId( player_id );
        auto keeper = _data_keeper.Create( player_id );
        keeper->_zone_id = zone_id;
        keeper->_player_id = player_id;
        keeper->_save_flag = save_flag;
        keeper->_pb_object.CopyFrom( *player_data );
        keeper->_save_time = KFGlobal::Instance()->_game_time + __KEEPER_SAVE_TIMER__;

        SaveKeeperData( zone_id, player_id, player_data, save_flag );
        return true;
    }

    void KFDataClientModule::SaveKeeperData( uint32 zone_id, uint64 player_id, const KFMsg::PBObject* player_data, uint32 save_flag )
    {
        __LOG_INFO__( "save player=[{}] req", player_id );

        KFMsg::S2SSavePlayerToDataReq req;
        req.set_id( player_id );
        req.set_zoneid( zone_id );
        req.set_flag( save_flag );
        req.mutable_data()->CopyFrom( *player_data );
        auto ok = _kf_route->SendToRand( player_id, __STRING__( data ), KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "save player=[{}] failed", player_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleSavePlayerToGameAck, KFMsg::S2SSavePlayerToGameAck )
    {
        _data_keeper.Remove( kfmsg->id() );
        __LOG_INFO__( "save player=[{}] ok", kfmsg->id() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataClientModule::QueryPlayerData( uint64 send_id, uint64 player_id )
    {
        auto keeper = _data_keeper.Find( player_id );
        if ( keeper != nullptr )
        {
            _query_player_function.Call( KFMsg::Ok, send_id, &keeper->_pb_object );
            return true;
        }

        KFMsg::S2SQueryPlayerToDataReq req;
        req.set_playerid( player_id );
        req.set_zoneid( CalcZoneId( player_id ) );
        return _kf_route->SendToRand( send_id, __STRING__( data ), KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFDataClientModule::HandleQueryPlayerToGameAck, KFMsg::S2SQueryPlayerToGameAck )
    {
        auto player_id = __ROUTE_RECV_ID__;
        _query_player_function.Call( kfmsg->result(), player_id, &kfmsg->playerdata() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
