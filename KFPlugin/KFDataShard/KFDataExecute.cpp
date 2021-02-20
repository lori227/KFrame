#include "KFDataExecute.hpp"
#include "KFRedis/KFRedisInterface.h"
#include "KFMongo/KFMongoInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    // 每次间隔的时间
#define __PER_SAVE_TIME__ 10000
#define __REDIS_DATA_DRIVER__( zone_id ) _kf_redis->Create( __STRING__( data ), zone_id )
#define __MYSQL_DATA_DRIVER__( zone_id ) _kf_mysql->Create( __STRING__( data ), zone_id )
#define __MONGO_DATA_DRIVER__( zone_id ) _kf_mongo->Create( __STRING__( data ), zone_id )
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    void KFDataKeeper::SetSaveTime( uint64 now_time )
    {
        ++_save_count;
        _next_save_time = now_time + ( _save_count * __PER_SAVE_TIME__ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFDataExecute::InitExecute( const KFDataSaveSetting* kfsetting )
    {
        _kf_setting = kfsetting;
        InitExecute();
    }

    void KFDataExecute::RunDataKeeper()
    {
        UInt64Set removes;
        auto now_time = KFGlobal::Instance()->_real_time;
        for ( auto& iter : _data_keeper._objects )
        {
            auto keeper = iter.second;
            if ( keeper->_next_save_time <= now_time )
            {
                keeper->SetSaveTime( now_time );
                auto ok = SaveData( keeper->_zone_id, keeper->_player_id, keeper->_player_data, keeper->_save_flag );
                if ( ok )
                {
                    removes.insert( keeper->_player_id );
                }
            }
        }

        // 删除
        for ( auto playerid : removes )
        {
            _data_keeper.Remove( playerid );
        }
    }

    void KFDataExecute::SavePlayerData( uint32 zone_id, uint64 playerid, const KFMsg::PBObject* pbobject, uint32 saveflag )
    {
        // 没有保存标记
        if ( !KFUtility::HaveBitMask( _kf_setting->_save_flag, saveflag ) )
        {
            return;
        }

        auto playerdata = KFProto::Serialize( pbobject, _kf_setting->_compress_type, _kf_setting->_compress_level, true );
        if ( playerdata == _invalid_string )
        {
            return __LOG_ERROR__( "player[{}:{}] serialize failed", zone_id, playerid );
        }

        // 不在keeper中
        auto keeper = _data_keeper.Find( playerid );
        if ( keeper == nullptr )
        {
            auto ok = SaveData( zone_id, playerid, playerdata, saveflag );
            if ( ok )
            {
                return;
            }
        }

        // 保存失败 先缓存下来
        keeper = _data_keeper.Create( playerid );
        keeper->_zone_id = zone_id;
        keeper->_player_id = playerid;
        keeper->_save_flag = saveflag;
        keeper->_player_data = playerdata;
        if ( keeper->_next_save_time == 0 )
        {
            keeper->SetSaveTime( KFGlobal::Instance()->_real_time );
        }
    }

    KFResult< std::string >::UniqueType KFDataExecute::LoadPlayerData( uint32 zone_id, uint64 playeid )
    {
        auto keeper = _data_keeper.Find( playeid );
        if ( keeper != nullptr )
        {
            __NEW_RESULT__( std::string );
            result->_value = keeper->_player_data;
            return result;
        }

        return LoadData( zone_id, playeid );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRedisDataExecute::SaveData( uint32 zone_id, uint64 playerid, const std::string& playerdata, uint32 saveflag )
    {
        auto redisdriver = __REDIS_DATA_DRIVER__( zone_id );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis", zone_id, playerid );
            return false;
        }

        auto strplayerkey = __DATABASE_KEY_2__( __STRING__( player ), playerid );
        auto kfresult = redisdriver->HSet( strplayerkey, __STRING__( data ), playerdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "redis player[{}:{}] save failed", zone_id, playerid );
            return false;
        }

        if ( _kf_setting->_cache_time != 0u && saveflag == KFSaveEnum::OfflineSave )
        {
            // 设置缓存时间
            redisdriver->Expire( strplayerkey, _kf_setting->_cache_time );
        }

        __LOG_INFO__( "redis player [{}:{}] size=[{}] save ok", zone_id, playerid, playerdata.size() );
        return true;
    }

    KFResult< std::string >::UniqueType KFRedisDataExecute::LoadData( uint32 zone_id, uint64 playerid )
    {
        auto redisdriver = __REDIS_DATA_DRIVER__( zone_id );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis", zone_id, playerid );
            return nullptr;
        }

        return redisdriver->HGet( __DATABASE_KEY_2__( __STRING__( player ), playerid ), __STRING__( data ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoDataExecute::SaveData( uint32 zone_id, uint64 playerid, const std::string& playerdata, uint32 saveflag )
    {
        auto mongodriver = __MONGO_DATA_DRIVER__( zone_id );
        if ( mongodriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find mongo", zone_id, playerid );
            return false;
        }

        mongodriver->CreateIndex( __STRING__( player ), MongoKeyword::_expire );
        auto ok = mongodriver->Insert( __STRING__( player ), playerid, __STRING__( data ), playerdata, true );
        if ( ok )
        {
            if ( _kf_setting->_cache_time != 0u && saveflag == KFSaveEnum::OfflineSave )
            {
                mongodriver->Expire( __STRING__( player ), playerid, _kf_setting->_cache_time );
            }
        }

        return ok;
    }

    KFResult< std::string >::UniqueType KFMongoDataExecute::LoadData( uint32 zone_id, uint64 playerid )
    {
        auto mongodriver = __MONGO_DATA_DRIVER__( zone_id );
        if ( mongodriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find mongo", zone_id, playerid );
            return nullptr;
        }

        return mongodriver->QueryString( __STRING__( player ), playerid, __STRING__( data ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLDataExecute::SaveData( uint32 zone_id, uint64 playerid, const std::string& playerdata, uint32 saveflag )
    {
        auto mysqldriver = __MYSQL_DATA_DRIVER__( zone_id );
        if ( mysqldriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find mysql", zone_id, playerid );
            return false;
        }

        StringMap values;
        values[ __STRING__( data ) ] = playerdata;
        values[ __STRING__( id ) ] = __TO_STRING__( playerid );

        auto ok = mysqldriver->Insert( __STRING__( player ), values );
        if ( !ok )
        {
            __LOG_ERROR__( "mysql player[{}:{}] save failed", zone_id, playerid );
            return false;
        }

        __LOG_INFO__( "mysql player [{}:{}] size=[{}] save ok", zone_id, playerid, playerdata.size() );
        return true;
    }

    KFResult< std::string >::UniqueType KFMySQLDataExecute::LoadData( uint32 zone_id, uint64 playerid )
    {
        auto mysqldriver = __MYSQL_DATA_DRIVER__( zone_id );
        if ( mysqldriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find mysql", zone_id, playerid );
            return nullptr;
        }

        return mysqldriver->QueryString( "select `{}` from `{}` where `{}`={};", __STRING__( data ), __STRING__( player ), __STRING__( id ), playerid );
    }
}