#include "KFRedisConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFRedisConfig::KFRedisConfig()
    {
        _default_redis_id = 1;
    }

    KFRedisConfig::~KFRedisConfig()
    {

    }

    void KFRedisConfig::AddRedisSetting( KFRedisSetting* kfsetting )
    {
        _redis_setting.Insert( kfsetting->_id, kfsetting );
    }

    const KFRedisSetting* KFRedisConfig::FindRedisSetting( uint32 id ) const
    {
        return _redis_setting.Find( id );
    }

    const KFRedisSetting* KFRedisConfig::FindRedisSetting( const std::string& field, uint32 logicid ) const
    {
        auto redisid = FindLogicRedisId( field, logicid );
        return FindRedisSetting( redisid );
    }

    uint32 KFRedisConfig::FindLogicRedisId( const std::string& filed, uint32 logicid ) const
    {
        auto key = LogicKey( filed, logicid );
        auto iter = _logic_redis_map.find( key );
        if ( iter == _logic_redis_map.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    bool KFRedisConfig::LoadConfig( const char* file )
    {
        _redis_setting.Clear();
        _logic_redis_map.clear();

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto databasenode = config.FindNode( "Redis" );
            if ( databasenode.IsValid() )
            {
                auto childnode = databasenode.FindNode( "Connection" );
                while ( childnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFRedisSetting );

                    kfsetting->_id = childnode.GetUInt32( "Id" );
                    kfsetting->_ip = childnode.GetString( "IP" );
                    kfsetting->_port = childnode.GetUInt32( "Port" );
                    kfsetting->_password = childnode.GetString( "Password" );
                    AddRedisSetting( kfsetting );

                    childnode.NextNode();
                }
            }

            auto redis = config.FindNode( "LogicRedis" );
            _default_redis_id = redis.GetUInt32( "DefaultRedis" );

            auto logic = redis.FindNode( "Logic" );
            while ( logic.IsValid() )
            {
                auto dataname = logic.GetString( "Filed" );
                auto logicid = logic.GetUInt32( "LogicId" );
                auto redisid = logic.GetUInt32( "Id" );

                auto key = LogicKey( dataname, logicid );
                _logic_redis_map[ key ] = redisid;

                logic.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}