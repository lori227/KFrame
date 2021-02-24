#include "KFChannel.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
#define __AUTH_REDIS_DRIVER__ _kf_redis->Create( __STRING__( auth ) )

    bool KFChannel::SavePayData( const std::string& order, StringMap& values )
    {
        auto redis_driver = __AUTH_REDIS_DRIVER__;

        auto order_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        if ( order_data->_value.empty() )
        {
            __LOG_ERROR__( "order=[{}] no record", order );
            return false;
        }

        auto pay_id = order_data->_value[ __STRING__( payid ) ];
        auto player_id = order_data->_value[ __STRING__( playerid ) ];
        auto query_order = order_data->_value[ __STRING__( payorder ) ];
        if ( pay_id.empty() || player_id.empty() || query_order != order )
        {
            __LOG_ERROR__( "order=[{}] payid=[{}] player_id=[{}] error", order, pay_id, player_id );
            return false;
        }

        // 保存充值纪录
        {
            values[ __STRING__( payid ) ] = pay_id;
            values[ __STRING__( payorder ) ] = order;
            values[ __STRING__( playerid ) ] = player_id;
        }

        // 纪录充值信息
        StringMap pay_values = order_data->_value;
        pay_values[ __STRING__( flag ) ] = "0";

        redis_driver->WriteMulti();
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( pay ), order ), pay_values );
        redis_driver->SAdd( __DATABASE_KEY_2__( __STRING__( paydata ), player_id ), order );
        auto result = redis_driver->WriteExec();
        if ( result->IsOk() )
        {
            // 删除订单信息
            redis_driver->Del( __DATABASE_KEY_2__( __STRING__( payorder ), order ) );
        }

        return result->IsOk();
    }
}