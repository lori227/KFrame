#include "KFPublicShardModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPublicShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_PUBLIC_REQ, &KFPublicShardModule::HandleUpdateDataToPublicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLEAR_ONLINE_TO_PUBLIC_REQ, &KFPublicShardModule::HandleClearOnlineToPublicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_PUBLIC_REQ, &KFPublicShardModule::HandleQueryBasicToPublicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_DATA_REQ, &KFPublicShardModule::HandleSetPlayerNameToDataReq );
    }

    void KFPublicShardModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_PUBLIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLEAR_ONLINE_TO_PUBLIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_PUBLIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_DATA_REQ );
    }

    void KFPublicShardModule::PrepareRun()
    {
        // 初始化redis
        _public_redis = _kf_redis->Create( __STRING__( public ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdateDataToPublicReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateDataToPublicReq );

        auto playerid = __ROUTE_SEND_ID__;
        auto pbdata = &kfmsg.pbdata();

        StringMap values;
        __PROTO_TO_MAP__( pbdata, values );
        _public_redis->Update( values, "hmset {}:{}", __STRING__( public ), playerid );

        auto status = KFUtility::ToValue< uint32 >( values[ __STRING__( status ) ] );
        if ( status == KFMsg::FrameOnlineStatus )
        {
            _public_redis->Execute( "sadd {}:{} {}", __STRING__( onlinelist ), __ROUTE_SERVER_ID__, playerid );
        }
        else if ( status == KFMsg::FrameOfflineStatus )
        {
            _public_redis->Execute( "srem {}:{} {}", __STRING__( onlinelist ), __ROUTE_SERVER_ID__, playerid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleClearOnlineToPublicReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClearOnlineToPublicReq );

        auto kfplayerlist = _public_redis->QueryList( "smembers {}:{}", __STRING__( onlinelist ), kfmsg.serverid() );
        if ( kfplayerlist->_value.empty() )
        {
            return;
        }

        for ( auto& id : kfplayerlist->_value )
        {
            auto queryserverid = _public_redis->QueryUInt64( "hget {}:{} {}", __STRING__( public ), id, __STRING__( serverid ) );
            if ( queryserverid->_value == kfmsg.serverid() )
            {
                _public_redis->Execute( "hmset {}:{} {} {} {} {}", __STRING__( public ), id,
                                        __STRING__( serverid ), 0,
                                        __STRING__( status ), KFMsg::FrameOnlineStatus );
            }

        }

        _public_redis->Execute( "del {}:{}", __STRING__( onlinelist ), kfmsg.serverid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryBasicToPublicReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryBasicToPublicReq );

        KFMsg::S2SQueryBasicToGameAck ack;
        ack.set_name( kfmsg.name() );

        auto& namekey = FormatNameKey( kfmsg.zoneid(), kfmsg.name() );
        auto queryid = _public_redis->QueryUInt64( "get {}", namekey );
        if ( !queryid->IsOk() )
        {
            ack.set_result( KFMsg::NameDatabaseBusy );
        }
        else
        {
            if ( queryid->_value == _invalid_int )
            {
                ack.set_result( KFMsg::QueryBasicNotExist );
            }
            else
            {
                auto querydata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), queryid->_value );
                if ( !querydata->IsOk() )
                {
                    ack.set_result( KFMsg::PublicDatabaseBusy );
                }
                else
                {
                    auto pbdata = ack.mutable_pbdata();
                    for ( auto& iter : querydata->_value )
                    {
                        ( *pbdata )[ iter.first ] = iter.second;
                    }
                }
            }
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_BASIC_TO_GAME_ACK, &ack );
    }


    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleSetPlayerNameToDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameToDataReq );

        // 先查询名字
        uint32 result = SetPlayerName( kfmsg.zoneid(), kfmsg.playerid(), kfmsg.oldname(), kfmsg.newname() );

        KFMsg::S2SSetPlayerNameToGameAck ack;
        ack.set_result( result );
        ack.set_name( kfmsg.newname() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_costdata( kfmsg.costdata() );
        _kf_route->SendToRoute( route, KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK, &ack );
    }

    const std::string& KFPublicShardModule::FormatNameKey( uint32 zoneid, const std::string& name )
    {
        static std::string _name = "";
        _name = __FORMAT__( "{}:{}:{}:{}", __STRING__( player ), __STRING__( name ), zoneid, name );
        return _name;
    }

    uint32 KFPublicShardModule::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto newnamekey = FormatNameKey( zoneid, newname );
        auto kfplayerid = _public_redis->QueryUInt64( "get {}", newnamekey );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto kfresult = _public_redis->Execute( "set {} {}", newnamekey, playerid );
            if ( !kfresult->IsOk() )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                auto oldnamekey = FormatNameKey( zoneid, oldname );
                _public_redis->Execute( "del {}", oldnamekey );
            }
        }
        else if ( kfplayerid->_value != playerid )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }
}