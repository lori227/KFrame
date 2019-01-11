#include "KFDataShardModule.h"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
#define __DATA_REDIS_DRIVER__( zoneid ) _kf_redis->Create( __KF_STRING__( data ), _invalid_int )

    void KFDataShardModule::BeforeRun()
    {
        __REGISTER_ROUTE_CONNECTION_FUNCTION__( &KFDataShardModule::OnRouteConnection );
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDataShardModule::OnTimerSaveDataKeeper );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ, &KFDataShardModule::HandleSavePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ, &KFDataShardModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &KFDataShardModule::HandleLoginLoadPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ, &KFDataShardModule::HandleSetPlayerNameReq );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDataShardModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_ROUTE_CONNECTION_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ );
    }

    void KFDataShardModule::OnceRun()
    {
        _name_redis_driver = _kf_redis->Create( __KF_STRING__( name ) );
    }

    __KF_ROUTE_CONNECTION_FUNCTION__( KFDataShardModule::OnRouteConnection )
    {
        RouteObjectList objectlist;
        _kf_route->SyncObject( __KF_STRING__( data ), objectlist );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataShardModule::LoadPlayerData( uint32 zoneid, uint64 playerid, KFMsg::PBObject* pbobject )
    {
        // 先判断在keeper中是否存在
        auto kfkeeper = _kf_data_keeper.Find( playerid );
        if ( kfkeeper != nullptr )
        {
            pbobject->CopyFrom( kfkeeper->_pb_object );
            return true;
        }

        auto redisdriver = __DATA_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis!", zoneid, playerid );
            return false;
        }

        auto kfresult = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( player ), playerid, __KF_STRING__( data ) );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "player[{}:{}] query failed!", zoneid, playerid );
            return false;
        }

        if ( !kfresult->_value.empty() )
        {
            auto ok = KFProto::Parse( pbobject, kfresult->_value, KFCompressEnum::Compress );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] parse failed!", zoneid, playerid );
                return false;
            }
        }

        return true;
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject )
    {
        auto strdata = KFProto::Serialize( pbobject, KFCompressEnum::Compress );
        if ( strdata == _invalid_str )
        {
            __LOG_ERROR__( "player[{}:{}] serialize failed!", zoneid, playerid );
            return false;
        }

        auto redisdriver = __DATA_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis!", zoneid, playerid );
            return false;
        }

        auto kfresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( player ), playerid, __KF_STRING__( data ), strdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "player[{}:{}] save failed!", zoneid, playerid );
            return false;
        }

        __LOG_DEBUG__( "player [{}:{}] save ok!", zoneid, playerid );
        return true;
    }

    __KF_TIMER_FUNCTION__( KFDataShardModule::OnTimerSaveDataKeeper )
    {
        std::set< uint64 > removes;
        for ( auto iter : _kf_data_keeper._objects )
        {
            auto kfkeeper = iter.second;

            auto ok = SavePlayerData( kfkeeper->_zone_id, kfkeeper->_player_id, &kfkeeper->_pb_object );
            if ( ok )
            {
                removes.insert( iter.first );
            }
        }

        for ( auto playerid : removes )
        {
            _kf_data_keeper.Remove( playerid );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginLoadPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] loaddata!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        auto zoneid = KFUtility::CalcZoneId( pblogin->playerid() );

        KFMsg::S2SLoginLoadPlayerAck ack;
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        bool ok = LoadPlayerData( zoneid, pblogin->playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Ok );
        }
        else
        {
            ack.set_result( KFMsg::LoadDataFailed );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSavePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSavePlayerReq );

        auto zoneid = KFUtility::CalcZoneId( kfmsg.id() );
        auto ok = SavePlayerData( zoneid, kfmsg.id(), &kfmsg.data() );
        if ( ok )
        {
            _kf_data_keeper.Remove( kfmsg.id() );
        }
        else
        {
            // 保存失败 先缓存下来
            auto kfkeeper = _kf_data_keeper.Create( kfmsg.id() );
            kfkeeper->_player_id = kfmsg.id();
            kfkeeper->_zone_id = zoneid;
            kfkeeper->_pb_object.CopyFrom( kfmsg.data() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleQueryPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryPlayerReq );

        auto zoneid = KFUtility::CalcZoneId( kfmsg.playerid() );

        KFMsg::S2SQueryPlayerAck ack;
        auto ok = LoadPlayerData( zoneid, kfmsg.playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Ok );
        }
        else
        {
            ack.set_result( KFMsg::QueryPlayerFailed );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_PLAYER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSetPlayerNameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameReq );

        // 先查询名字
        uint32 result = SetPlayerName( kfmsg.playerid(), kfmsg.oldname(), kfmsg.newname() );

        KFMsg::S2SSetPlayerNameAck ack;
        ack.set_result( result );
        ack.set_name( kfmsg.newname() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_itemguid( kfmsg.itemguid() );
        _kf_route->SendToRoute( route, KFMsg::S2S_SET_PLAYER_NAME_ACK, &ack );
    }

    uint32 KFDataShardModule::SetPlayerName( uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto kfplayerid = _name_redis_driver->QueryUInt64( "get {}:{}", __KF_STRING__( name ), newname );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto kfresult = _name_redis_driver->Execute( "set {}:{} {}", __KF_STRING__( name ), newname, playerid );
            if ( !kfresult->IsOk() )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                _name_redis_driver->Execute( "del {}:{}", __KF_STRING__( name ), oldname );
            }
        }
        else if ( kfplayerid->_value != playerid )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOK;
    }
}