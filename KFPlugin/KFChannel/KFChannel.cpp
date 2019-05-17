#include "KFChannel.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
#define __AUTH_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( auth ) )

    bool KFChannel::SavePayData( const std::string& order, MapString& values )
    {
        auto redisdriver = __AUTH_REDIS_DRIVER__;

        auto kforderdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( order ), order );
        if ( kforderdata->_value.empty() )
        {
            __LOG_ERROR__( "order=[{}] no record!", order );
            return false;
        }

        auto payid = kforderdata->_value[ __KF_STRING__( payid ) ];
        auto playerid = kforderdata->_value[ __KF_STRING__( playerid ) ];
        auto queryorder = kforderdata->_value[ __KF_STRING__( order ) ];
        if ( payid.empty() || playerid.empty() || queryorder != order )
        {
            __LOG_ERROR__( "order=[{}] payid=[{}] playerid=[{}] error!", order, payid, playerid );
            return false;
        }

        // 保存充值纪录
        {
            values[ __KF_STRING__( payid ) ] = payid;
            values[ __KF_STRING__( order ) ] = order;
            values[ __KF_STRING__( playerid ) ] = playerid;
        }

        // 纪录充值信息
        MapString payvalues = kforderdata->_value;
        payvalues[ __KF_STRING__( flag ) ] = "0";

        redisdriver->Append( payvalues, "hmset {}:{}", __KF_STRING__( pay ), order );
        redisdriver->Append( "sadd {}:{} {}", __KF_STRING__( paydata ), playerid, order );
        auto kfresult = redisdriver->Pipeline();
        if ( kfresult->IsOk() )
        {
            // 删除订单信息
            redisdriver->Execute( "del {}:{}", __KF_STRING__( order ), order );
        }

        return kfresult->IsOk();
    }
}