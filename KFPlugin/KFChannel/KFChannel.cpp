#include "KFChannel.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
#define __AUTH_REDIS_DRIVER__ _kf_redis->Create( __STRING__( auth ) )

    bool KFChannel::SavePayData( const std::string& order, StringMap& values )
    {
        auto redis_driver = __AUTH_REDIS_DRIVER__;

        auto kforderdata = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        if ( kforderdata->_value.empty() )
        {
            __LOG_ERROR__( "order=[{}] no record", order );
            return false;
        }

        auto payid = kforderdata->_value[ __STRING__( payid ) ];
        auto player_id = kforderdata->_value[ __STRING__( playerid ) ];
        auto queryorder = kforderdata->_value[ __STRING__( payorder ) ];
        if ( payid.empty() || player_id.empty() || queryorder != order )
        {
            __LOG_ERROR__( "order=[{}] payid=[{}] player_id=[{}] error", order, payid, player_id );
            return false;
        }

        // 保存充值纪录
        {
            values[ __STRING__( payid ) ] = payid;
            values[ __STRING__( payorder ) ] = order;
            values[ __STRING__( playerid ) ] = player_id;
        }

        // 纪录充值信息
        StringMap payvalues = kforderdata->_value;
        payvalues[ __STRING__( flag ) ] = "0";

        redis_driver->WriteMulti();
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( pay ), order ), payvalues );
        redis_driver->SAdd( __DATABASE_KEY_2__( __STRING__( paydata ), player_id ), order );
        auto kfresult = redis_driver->WriteExec();
        if ( kfresult->IsOk() )
        {
            // 删除订单信息
            redis_driver->Del( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        }

        return kfresult->IsOk();
    }
}