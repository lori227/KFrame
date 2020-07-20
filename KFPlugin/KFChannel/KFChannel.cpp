#include "KFChannel.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
#define __AUTH_REDIS_DRIVER__ _kf_redis->Create( __STRING__( auth ) )

    bool KFChannel::SavePayData( const std::string& order, StringMap& values )
    {
        auto redisdriver = __AUTH_REDIS_DRIVER__;

        auto kforderdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        if ( kforderdata->_value.empty() )
        {
            __LOG_ERROR__( "order=[{}] no record", order );
            return false;
        }

        auto payid = kforderdata->_value[ __STRING__( payid ) ];
        auto playerid = kforderdata->_value[ __STRING__( playerid ) ];
        auto queryorder = kforderdata->_value[ __STRING__( payorder ) ];
        if ( payid.empty() || playerid.empty() || queryorder != order )
        {
            __LOG_ERROR__( "order=[{}] payid=[{}] playerid=[{}] error", order, payid, playerid );
            return false;
        }

        // 保存充值纪录
        {
            values[ __STRING__( payid ) ] = payid;
            values[ __STRING__( payorder ) ] = order;
            values[ __STRING__( playerid ) ] = playerid;
        }

        // 纪录充值信息
        StringMap payvalues = kforderdata->_value;
        payvalues[ __STRING__( flag ) ] = "0";

        redisdriver->WriteMulti();
        redisdriver->HMSet( __DATABASE_KEY_2__( __STRING__( pay ), order ), payvalues );
        redisdriver->SAdd( __DATABASE_KEY_2__( __STRING__( paydata ), playerid ), order );
        auto kfresult = redisdriver->WriteExec();
        if ( kfresult->IsOk() )
        {
            // 删除订单信息
            redisdriver->Del( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        }

        return kfresult->IsOk();
    }
}