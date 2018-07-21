#include "KFDataShardModule.h"
#include "KFCompress/KFCompress.h"
#include "KFDataShardConfig.h"

namespace KFrame
{
    static KFRedisDriver* _kf_account_redis = nullptr;
#define __ACCOUNT_REDIS_DRIVER__ _kf_account_redis
#define __PLAYER_REDIS_DRIVER__( zoneid ) _kf_redis->CreateExecute( KFField::_player, zoneid )

#define __REGISTER_DATA_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_DATA_MESSAGE__ __UNREGISTER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )

    KFDataShardModule::KFDataShardModule()
    {

    }

    KFDataShardModule::~KFDataShardModule()
    {

    }

    void KFDataShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_data_config, true );
    }

    void KFDataShardModule::BeforeRun()
    {
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFDataShardModule::OnServerDiscoverClient );
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDataShardModule::OnTimerSaveDataKeeper );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ, &KFDataShardModule::HandleSavePlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_DELETE_PLAYER_REQ, &KFDataShardModule::HandleDeletePlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_REQ, &KFDataShardModule::HandleLoginQueryPlayerIdReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &KFDataShardModule::HandleLoginLoadPlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ, &KFDataShardModule::HandleQueryPlayerReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDataShardModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_DELETE_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ );
    }

    void KFDataShardModule::OnceRun()
    {
        _kf_account_redis = _kf_redis->CreateExecute( KFField::_account );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFDataShardModule::OnServerDiscoverClient )
    {
        // 分区信息同步到代理服务器
        auto zonelist = _kf_data_config->FindZoneId( KFGlobal::Instance()->_app_id );
        if ( zonelist == nullptr )
        {
            return KFLogger::LogSystem( KFLogger::Error, "[%s] server[%u] can't find zone list",
                                        __FUNCTION__, KFGlobal::Instance()->_app_id );
        }

        KFMsg::S2SUpdateZoneToProxyReq req;
        for ( auto zoneid : *zonelist )
        {
            req.add_zoneid( zoneid );
        }

        _kf_cluster_shard->SendMessageToProxy( handleid, KFMsg::S2S_UPDATE_ZONE_TO_PROXY_REQ, &req );
    }

    uint32 KFDataShardModule::QueryCreatePlayerId( uint32 channel, uint32 accountid, uint32 zoneid, uint32 logiczoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        uint32 playerid = 0;
        if ( !redisdriver->UInt32Execute( playerid, "hget %s:%u %u", KFField::_user.c_str(), accountid, logiczoneid ) )
        {
            return 0;
        }

        if ( playerid != 0 )
        {
            return playerid;
        }

        // 创建playerid
        uint64 newuserid = 0;
        redisdriver->UInt64Execute( newuserid, "incr %s:%u", KFField::_player_id_create.c_str(), zoneid );
        if ( newuserid == 0 )
        {
            return 0;
        }

        newuserid += 10000;
        playerid = KFUtility::CalcPlayerid( static_cast< uint32 >( newuserid ), zoneid );
        if ( !redisdriver->VoidExecute( "hset %s:%u %u %u", KFField::_user.c_str(), accountid, logiczoneid, playerid ) )
        {
            return 0;
        }

        return playerid;
    }

    void KFDataShardModule::CreatePlayer( uint32 channel, uint32 accountid, uint32 zoneid, uint32 playerid )
    {
        auto redisdriver = __PLAYER_REDIS_DRIVER__( zoneid  );

        // 查询角色是否存在, 已经存在, 不需要创建
        uint32 queryaccountid = 0;
        redisdriver->UInt32Execute( queryaccountid, "hget %s:%u %s", KFField::_player.c_str(), playerid, KFField::_account_id.c_str() );
        if ( queryaccountid != 0 )
        {
            return;
        }

        // 保存玩家信息
        MapString values;
        values[ KFField::_zone_id ] = KFUtility::ToString( zoneid );
        values[ KFField::_channel ] = KFUtility::ToString( channel );
        values[ KFField::_account_id ] = KFUtility::ToString( accountid );
        redisdriver->VoidExecute( values, "hmset %s:%u", KFField::_player.c_str(), playerid );

        // 保存到列表中
        redisdriver->VoidExecute( "sadd %s:%u %u", KFField::_player_list.c_str(), zoneid, playerid );
    }

    bool KFDataShardModule::LoadPlayerData( uint32 zoneid, uint32 id, KFMsg::PBObject* pbobject )
    {
        // 先判断在keeper中是否存在
        auto kfkeeper = _kf_data_keeper.Find( id );
        if ( kfkeeper != nullptr )
        {
            pbobject->CopyFrom( kfkeeper->_pb_object );
            return true;
        }

        auto redisdriver = __PLAYER_REDIS_DRIVER__( zoneid );

        std::string querystring = "";
        if ( !redisdriver->StringExecute( querystring, "hget %s:%u %s", KFField::_player.c_str(), id, KFField::_data.c_str() ) )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u:%u] redis failed!",
                                __FUNCTION_LINE__, zoneid, id );
            return false;
        }

        if ( !querystring.empty() )
        {
            std::string datastring;
            KFCompress::UnCompress( querystring, datastring );
            if ( !pbobject->ParseFromString( datastring ) )
            {
                KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u:%u] parse failed!",
                                    __FUNCTION_LINE__, zoneid, id );
                return false;
            }
        }

        return true;
    }

    void KFDataShardModule::LoadExtendData( uint32 accountid, KFMsg::PBStrings* extenddata )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        MapString values;
        redisdriver->MapExecute( values, "hgetall %s:%u", KFField::_extend.c_str(), accountid );
        for ( auto& iter : values )
        {
            auto pbstring = extenddata->add_pbstring();
            pbstring->set_name( iter.first );
            pbstring->set_value( iter.second );
        }
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint32 id, const KFMsg::PBObject* pbobject )
    {
        auto datastring = pbobject->SerializeAsString();

        std::string compressstring;
        auto result = KFCompress::Compress( datastring, compressstring );
        if ( result != 0 )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u:%u] compress failed[%d]!",
                                __FUNCTION_LINE__, zoneid, id, result );
            return false;
        }

        auto redisdriver = __PLAYER_REDIS_DRIVER__( zoneid );
        if ( !redisdriver->VoidExecute( "hset %s:%u %s %s", KFField::_player.c_str(), id, KFField::_data.c_str(), compressstring.c_str() ) )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u:%u] redis failed!",
                                __FUNCTION_LINE__, zoneid, id );
            return false;
        }

        KFLogger::LogLogic( KFLogger::Info, "[%s:%u] player [%u:%u] save ok!",
                            __FUNCTION_LINE__, zoneid, id );
        return true;
    }

    __KF_TIMER_FUNCTION__( KFDataShardModule::OnTimerSaveDataKeeper )
    {
        std::set< uint32 > removes;
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

    bool KFDataShardModule::DeletePlayerData( uint32 zoneid, uint32 id )
    {
#ifndef __KF_DEBUG__
        return true;
#endif
        auto redisdriver = __PLAYER_REDIS_DRIVER__( zoneid );

        if ( !redisdriver->VoidExecute( "del %s:%u", KFField::_player.c_str(), id ) )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u:%u] redis failed!",
                                __FUNCTION_LINE__, zoneid, id );
            return false;
        }

        // 从列表中删除
        if ( !redisdriver->VoidExecute( "srem %s:%u %u", KFField::_player_list.c_str(), zoneid, id ) )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player list [%u:%u] redis failed!",
                                __FUNCTION_LINE__, zoneid, id );
            return false;
        }

        KFLogger::LogLogic( KFLogger::Info, "[%s:%u] player[%u:%u] delete ok!",
                            __FUNCTION_LINE__, zoneid, id );
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginQueryPlayerIdReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryPlayerIdReq );

        auto channel = kfmsg.channel();
        auto accountid = kfmsg.accountid();
        auto zoneid = kfmsg.zoneid();
        auto logiczoneid = kfmsg.logiczoneid();

        auto playerid = QueryCreatePlayerId( channel, accountid, zoneid, logiczoneid );
        if ( playerid != 0 )
        {
            CreatePlayer( channel, accountid, zoneid, playerid );
        }

        KFMsg::S2SLoginQueryPlayerIdAck ack;
        ack.set_loginserverid( kfmsg.loginserverid() );
        ack.set_gateid( kfmsg.gateid() );
        ack.set_accountid( kfmsg.accountid() );
        ack.set_token( kfmsg.token() );
        ack.set_playerid( playerid );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginLoadPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerReq );

        KFMsg::S2SLoginLoadPlayerAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_gateid( kfmsg.gateid() );

        bool ok = LoadPlayerData( kfmsg.zoneid(), kfmsg.playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Success );
            LoadExtendData( kfmsg.accountid(), ack.mutable_extenddata() );
        }
        else
        {
            ack.set_result( KFMsg::LoadDataFailed );
        }
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSavePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSavePlayerReq );

        auto ok = SavePlayerData( kfmsg.zoneid(), kfmsg.id(), &kfmsg.data() );
        if ( ok )
        {
            _kf_data_keeper.Remove( kfmsg.id() );
        }
        else
        {
            // 保存失败 先缓存下来
            auto kfkeeper = _kf_data_keeper.Create( kfmsg.id() );
            kfkeeper->_player_id = kfmsg.id();
            kfkeeper->_zone_id = kfmsg.zoneid();
            kfkeeper->_pb_object.CopyFrom( kfmsg.data() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleDeletePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeletePlayerReq );

        DeletePlayerData( kfmsg.zoneid(), kfmsg.id() );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleQueryPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryPlayerReq );

        KFMsg::S2SQueryPlayerAck ack;
        ack.set_playerid( kfmsg.playerid() );

        LoadPlayerData( kfmsg.zoneid(), kfmsg.queryid(), ack.mutable_pbobject() );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_PLAYER_ACK, &ack );
    }

}