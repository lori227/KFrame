#include "KFAuthModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( dir ) )
#define __AUTH_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( auth ) )

    void KFAuthModule::BeforeRun()
    {
        // zone
        __REGISTER_HTTP__( __KF_STRING__( zoneregister ), false, &KFAuthModule::HandleZoneRegister );
        __REGISTER_HTTP__( __KF_STRING__( zoneupdate ), false, &KFAuthModule::HandleZoneUpdate );
        __REGISTER_HTTP__( __KF_STRING__( zoneremove ), false, &KFAuthModule::HandleZoneRemove );
        __REGISTER_HTTP__( __KF_STRING__( zonelist ), false, &KFAuthModule::HandleQueryZoneList );
        __REGISTER_HTTP__( __KF_STRING__( zoneip ), false, &KFAuthModule::HandleQueryZoneIp );
        __REGISTER_HTTP__( __KF_STRING__( zonebalance ), false, &KFAuthModule::HandleZoneBalance );
        __REGISTER_HTTP__( __KF_STRING__( worldregister ), false, &KFAuthModule::HandleWorldRegister );

        // auth
        __REGISTER_HTTP__( __KF_STRING__( auth ), false, &KFAuthModule::HandleAuthLogin );
        __REGISTER_HTTP__( __KF_STRING__( verify ), false, &KFAuthModule::HandleVerifyToken );

        // pay
        __REGISTER_HTTP__( __KF_STRING__( applyorder ), false, &KFAuthModule::HandleApplyOrder );
        __REGISTER_HTTP__( __KF_STRING__( removeorder ), false, &KFAuthModule::HandleRemoveOrder );
        __REGISTER_HTTP__( __KF_STRING__( querypay ), false, &KFAuthModule::HandleQueryPay );
        __REGISTER_HTTP__( __KF_STRING__( finishpay ), false, &KFAuthModule::HandleFinishPay );
        __REGISTER_HTTP__( __KF_STRING__( internalpay ), false, &KFAuthModule::HandleInternalPay );

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        // zone
        __UN_HTTP__( __KF_STRING__( zoneregister ) );
        __UN_HTTP__( __KF_STRING__( zoneupdate ) );
        __UN_HTTP__( __KF_STRING__( zoneremove ) );
        __UN_HTTP__( __KF_STRING__( zonelist ) );
        __UN_HTTP__( __KF_STRING__( zoneip ) );
        __UN_HTTP__( __KF_STRING__( zonebalance ) );
        __UN_HTTP__( __KF_STRING__( worldregister ) );

        // auth
        __UN_HTTP__( __KF_STRING__( auth ) );
        __UN_HTTP__( __KF_STRING__( verify ) );

        // pay
        __UN_HTTP__( __KF_STRING__( applyorder ) );
        __UN_HTTP__( __KF_STRING__( removeorder ) );
        __UN_HTTP__( __KF_STRING__( querypay ) );
        __UN_HTTP__( __KF_STRING__( finishpay ) );
        __UN_HTTP__( __KF_STRING__( internalpay ) );
        ///////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneBalance )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );
        auto count = __JSON_GET_UINT64__( request, __KF_STRING__( count ) );
        auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );

        auto redisdriver = __DIR_REDIS_DRIVER__;
        redisdriver->Execute( "zincrby {} {} {}", __KF_STRING__( zonelist ), count, zoneid );
        return _kf_http_server->SendCode( KFMsg::Ok );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneRegister )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );

        MapString values;
        __JSON_TO_MAP__( request, values );

        // 先保存小区基本信息
        auto redisdriver = __DIR_REDIS_DRIVER__;
        redisdriver->Append( "zincrby {} 0 {}", __KF_STRING__( zonelist ), zoneid );
        redisdriver->Append( values, "hmset {}:{}", __KF_STRING__( zone ), zoneid );
        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            return _kf_http_server->SendCode( KFMsg::Error );
        }

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneUpdate )
    {
        __JSON_PARSE_STRING__( request, data );

        // 判断最小负载
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );
        auto count = __JSON_GET_UINT32__( request, __KF_STRING__( count ) );

        MapString values;
        values[ __KF_STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( zone ), zoneid, __KF_STRING__( count ) );
        if ( kfresult->_value >= count )
        {
            __JSON_TO_MAP__( request, values );
        }

        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( zone ), zoneid );
        return _kf_http_client->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneRemove )
    {
        __JSON_PARSE_STRING__( request, data );

        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );
        auto appid = __JSON_GET_STRING__( request, __KF_STRING__( appid ) );

        auto kfresult = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( zone ), zoneid, __KF_STRING__( appid ) );
        if ( kfresult->_value == appid )
        {
            // 如果是本小区, 更新数量到最大, 方便后面更新
            redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( zone ), zoneid, __KF_STRING__( count ), __MAX_UINT32__ );
        }

        return _kf_http_client->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryZoneList )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "zrange {} 0 -1", __KF_STRING__( zonelist ) );
        if ( !kflist->IsOk() )
        {
            return _kf_http_server->SendCode( KFMsg::AuthDatabaseBusy );
        }

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( kfarray );
        for ( auto& strid : kflist->_value )
        {
            auto kfmap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zone ), strid );
            if ( kfmap->_value.empty() )
            {
                __JSON_OBJECT__( kfzone );
                __JSON_SET_VALUE__( kfzone, __KF_STRING__( zoneid ), KFUtility::ToValue<uint32>( kfmap->_value[ __KF_STRING__( zoneid ) ] ) );
                __JSON_SET_VALUE__( kfzone, __KF_STRING__( name ), kfmap->_value[ __KF_STRING__( name ) ] );
                __JSON_ADD_VALUE__( kfarray, kfzone );
            }
        }
        __JSON_SET_VALUE__( response, __KF_STRING__( zonelist ), kfarray );

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryZoneIp )
    {
        __JSON_PARSE_STRING__( request, data );

        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );

        // 查询登录信息
        auto kfmap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zone ), zoneid );
        if ( !kfmap->IsOk() )
        {
            return _kf_http_server->SendCode( KFMsg::ZoneDatabaseBusy );
        }

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( zoneid ), zoneid );
        __JSON_SET_VALUE__( response, __KF_STRING__( ip ), kfmap->_value[ __KF_STRING__( ip ) ] );
        __JSON_SET_VALUE__( response, __KF_STRING__( port ), KFUtility::ToValue( kfmap->_value[ __KF_STRING__( port ) ] ) );

        return _kf_http_server->SendResponse( response );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleWorldRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto appid = __JSON_GET_STRING__( request, __KF_STRING__( world ) );
        auto url = __JSON_GET_STRING__( request, __KF_STRING__( url ) );

        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "sadd {} {}", __KF_STRING__( worldurl ), url );
        if ( !kfresult->IsOk() )
        {
            return _kf_http_server->SendCode( KFMsg::Error );
        }

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthLogin )
    {
        // 渠道验证
        auto authdata = _kf_channel->AuthLogin( data );

        __JSON_PARSE_STRING__( authjson, authdata );
        auto retcode = _kf_http_server->GetCode( authjson );
        if ( retcode != KFMsg::Ok )
        {
            return _kf_http_server->SendCode( retcode );
        }

        auto account = __JSON_GET_STRING__( authjson, __KF_STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( authjson, __KF_STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::AccountIsEmpty );
        }

        __LOG_DEBUG__( "account[{}] channel[{}] auth login!", account, channel );

        // 查询创建账号
        auto accountdata = QueryCreateAccount( account, channel );
        auto accountid = KFUtility::ToValue( accountdata[ __KF_STRING__( accountid ) ] );
        if ( accountid == 0 )
        {
            return _kf_http_server->SendCode( KFMsg::AuthServerBusy );
        }

        // 保存渠道的数据
        UpdateChannelData( accountid, channel, authjson );

        // 保存token
        auto token = SaveLoginToken( accountid, accountdata );

        // 查询小区信息, 返回给客户端
        return QueryZoneData( accountid, token, accountdata );
    }

    void KFAuthModule::UpdateChannelData( uint64 accountid, uint32 channel, KFJson& kfjson )
    {
        if ( !__JSON_HAS_MEMBER__( kfjson, __KF_STRING__( extend ) ) )
        {
            return;
        }

        auto& kfextend = kfjson[ __KF_STRING__( extend ) ];

        MapString values;
        __JSON_TO_MAP__( kfextend, values );

        auto redisdriver = __AUTH_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( extend ), accountid );
    }

    MapString KFAuthModule::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        MapString accountdata;
        auto redisdriver = __AUTH_REDIS_DRIVER__;

        // 先查询redis
        auto kfquery = redisdriver->QueryUInt64( "hget {}:{}:{} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ) );
        if ( !kfquery->IsOk() )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] query accountid failed!", account, channel );
            return accountdata;
        }

        auto accountid = kfquery->_value;
        if ( accountid != _invalid_int )
        {
            auto queryaccountdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), accountid );
            if ( !queryaccountdata->IsOk() || !queryaccountdata->_value.empty() )
            {
                return queryaccountdata->_value;
            }
        }
        else
        {
            accountid = KFGlobal::Instance()->MTMakeUUID( __KF_STRING__( account ) );
        }

        // 创建账号id
        accountdata[ __KF_STRING__( account ) ] = account;
        accountdata[ __KF_STRING__( channel ) ] = __TO_STRING__( channel );
        accountdata[ __KF_STRING__( accountid ) ] = __TO_STRING__( accountid );

        redisdriver->Append( "hset {}:{}:{} {} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ), accountid );
        redisdriver->Append( "sadd {} {}", __KF_STRING__( accountlist ), accountid );
        redisdriver->Append( accountdata, "hmset {}:{}", __KF_STRING__( accountid ), accountid );
        auto kfresult = redisdriver->Pipeline();
        if ( kfresult->IsOk() )
        {
            __LOG_DEBUG__( "create account successfully, account[{}] accountid[{}] channel[{}] ", account, accountid, channel );
        }
        else
        {
            // 失败清空数据
            accountdata.clear();
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed!", account, channel );
        }

        return accountdata;
    }

    std::string KFAuthModule::SaveLoginToken( uint64 accountid, MapString& accountdata )
    {
        // token 保留1个小时
        static auto _token_expire_time = 3600;
        static auto _token_serial = 0;
        auto redisdriver = __AUTH_REDIS_DRIVER__;

        auto channel = accountdata[ __KF_STRING__( channel ) ];
        auto account = accountdata[ __KF_STRING__( account ) ];
        auto md5temp = __FORMAT__( "{}:{}:{}:{}:{}", accountid, KFGlobal::Instance()->_game_time, channel, account, ++_token_serial );
        auto token = KFCrypto::Md5Encode( md5temp );

        auto tokenkey = __FORMAT__( "{}:{}", __KF_STRING__( token ), accountid );
        redisdriver->Append( "hmset {} {} {} {} {} {} {}",
                             tokenkey,
                             __KF_STRING__( token ), token,
                             __KF_STRING__( account ), account,
                             __KF_STRING__( channel ), channel );
        redisdriver->Append( "expire {} {}", tokenkey, _token_expire_time );
        redisdriver->Pipeline();

        return token;
    }

    bool KFAuthModule::KickAccountOffline( uint64 playerid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;

        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( playerid ), playerid );

        do
        {
            auto kfresult = redisdriver->QueryString( "srandmember {}", __KF_STRING__( worldurl ) );
            if ( kfresult->_value.empty() )
            {
                break;
            }

            auto kickurl = kfresult->_value + __KF_STRING__( kickonline );
            auto resultdata = _kf_http_client->STGet( kickurl, sendjson );

            __JSON_PARSE_STRING__( recvjson, resultdata );
            auto retcode = __JSON_GET_UINT32__( recvjson, __KF_STRING__( retcode ) );
            if ( retcode == KFMsg::Ok )
            {
                return true;
            }

            // 不能访问, 删除这个地址
            redisdriver->Execute( "srem {} {}", __KF_STRING__( worldurl ), kfresult->_value );
        } while ( true );

        return false;
    }

    uint32 KFAuthModule::BalanceAllocZoneId()
    {
        // todo:如果有推荐的服务器, 直接返回
        // 选择一个最小人数的分区
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zonelist = redisdriver->QueryList( "zrange {} 0 -1", __KF_STRING__( zonelist ) );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& strid : zonelist->_value )
        {
            auto querytime = redisdriver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( zone ), strid, __KF_STRING__( time ) );
            if ( KFGlobal::Instance()->_real_time < ( querytime->_value + 3600 ) )
            {
                return KFUtility::ToValue< uint32 >( strid );
            }
        }

        // 推荐的服务器
        return 1u;
    }

    std::string KFAuthModule::QueryZoneData( uint64 accountid, const std::string& token, MapString& accountdata )
    {
        // 上次登录的小区id
        auto zoneid = KFUtility::ToValue< uint32 >( accountdata[ __KF_STRING__( zoneid ) ] );
        if ( zoneid == _invalid_int )
        {
            // 没有登录, 说明是个新号, 找到负载最小的一个小区信息
            zoneid = BalanceAllocZoneId();
            if ( zoneid == _invalid_int )
            {
                return _kf_http_server->SendCode( KFMsg::ZoneServerBusy );
            }
        }

        // 小区信息
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfzonemap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zone ), zoneid );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( token ), token );
        __JSON_SET_VALUE__( response, __KF_STRING__( accountid ), accountid );

        __JSON_OBJECT__( kfzone );
        __JSON_SET_VALUE__( kfzone, __KF_STRING__( name ), kfzonemap->_value[ __KF_STRING__( name ) ] );
        __JSON_SET_VALUE__( kfzone, __KF_STRING__( ip ), kfzonemap->_value[ __KF_STRING__( ip ) ] );
        __JSON_SET_VALUE__( kfzone, __KF_STRING__( port ), KFUtility::ToValue<uint32>( kfzonemap->_value[ __KF_STRING__( port ) ] ) );
        __JSON_SET_VALUE__( kfzone, __KF_STRING__( zoneid ), KFUtility::ToValue<uint32>( kfzonemap->_value[ __KF_STRING__( zoneid ) ] ) );
        __JSON_SET_VALUE__( response, __KF_STRING__( zone ), kfzone );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleVerifyToken )
    {
        __JSON_PARSE_STRING__( request, data );

        auto redisdriver = __AUTH_REDIS_DRIVER__;

        auto token = __JSON_GET_STRING__( request, __KF_STRING__( token ) );
        auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );

        // 判断token是否正确
        auto tokenkey = __FORMAT__( "{}:{}", __KF_STRING__( token ), accountid );
        auto kftokenresult = redisdriver->QueryMap( "hgetall {}", tokenkey );
        if ( !kftokenresult->IsOk() )
        {
            return _kf_http_server->SendCode( KFMsg::AuthDatabaseBusy );
        }

        // 获得账号和渠道 小区信息
        auto querytoken = kftokenresult->_value[ __KF_STRING__( token ) ];
        auto account = kftokenresult->_value[ __KF_STRING__( account ) ];
        auto channel = KFUtility::ToValue< uint32 >( kftokenresult->_value[ __KF_STRING__( channel ) ] );
        if ( querytoken.empty() || querytoken != token || channel == _invalid_int )
        {
            return _kf_http_server->SendCode( KFMsg::LoginTokenError );
        }

        // 保存玩家登陆ip
        {
            MapString values;
            values[ __KF_STRING__( ip ) ] = __JSON_GET_STRING__( request, __KF_STRING__( ip ) );
            values[ __KF_STRING__( zoneid ) ] = __JSON_GET_STRING__( request, __KF_STRING__( zoneid ) );
            redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( accountid ), accountid );
        }

        // 返回结果
        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( token ), token );
        __JSON_SET_VALUE__( response, __KF_STRING__( channel ), channel );
        __JSON_SET_VALUE__( response, __KF_STRING__( account ), account );
        __JSON_SET_VALUE__( response, __KF_STRING__( accountid ), accountid );

        // 加载渠道数据
        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( extend ), accountid );
        if ( !kfresult->_value.empty() )
        {
            KFJson kfchanneljson;
            __JSON_FROM_MAP__( kfchanneljson, kfresult->_value );
            __JSON_SET_VALUE__( response, __KF_STRING__( channeldata ), kfchanneljson );
        }

        return _kf_http_server->SendResponse( response );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleApplyOrder )
    {
        __JSON_PARSE_STRING__( request, data );

        auto order = __JSON_GET_STRING__( request, __KF_STRING__( payorder ) );
        auto payid = __JSON_GET_STRING__( request, __KF_STRING__( payid ) );
        auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
        if ( playerid == _invalid_int || payid.empty() || order.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::PayDataError );
        }

        // 订单保存时间( 1个月 )
        static auto _order_expire_time = 3600 * 24 * 30;
        auto orderkey = __FORMAT__( "{}:{}", __KF_STRING__( payorder ), order );

        // 保存到数据库
        auto redisdriver = __AUTH_REDIS_DRIVER__;

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( playerid ), playerid );

        // 保存订单信息
        MapString values;
        __JSON_TO_MAP__( request, values );
        redisdriver->Append( values, "hmset {}", orderkey );
        redisdriver->Append( "expire {} {}", orderkey, _order_expire_time );
        auto kfresult = redisdriver->Pipeline();
        if ( kfresult->IsOk() )
        {
            __JSON_SET_VALUE__( response, __KF_STRING__( payid ), payid );
            __JSON_SET_VALUE__( response, __KF_STRING__( payorder ), order );
            __JSON_SET_VALUE__( response, __KF_STRING__( retcode ), KFMsg::Ok );
        }
        else
        {
            __JSON_SET_VALUE__( response, __KF_STRING__( retcode ), KFMsg::AuthDatabaseBusy );
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveOrder )
    {
        __JSON_PARSE_STRING__( request, data );

        // 删除订单
        auto order = __JSON_GET_STRING__( request, __KF_STRING__( payorder ) );

        auto redisdriver = __AUTH_REDIS_DRIVER__;
        redisdriver->Execute( "del {}:{}", __KF_STRING__( payorder ), order );

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryPay )
    {
        __JSON_PARSE_STRING__( request, data );
        auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( playerid ), playerid );

        // 查询玩家充值列表
        auto redisdriver = __AUTH_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryList( "smembers {}:{}", __KF_STRING__( paydata ), playerid );
        if ( !kfresult->_value.empty() )
        {
            __JSON_ARRAY__( payarray );
            for ( auto& order : kfresult->_value )
            {
                auto kfpaydata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( pay ), order );
                if ( !kfpaydata->_value.empty() )
                {
                    auto flag = KFUtility::ToValue( kfpaydata->_value[ __KF_STRING__( flag ) ] );
                    if ( flag == _invalid_int )
                    {
                        __JSON_OBJECT__( payjson );
                        __JSON_SET_VALUE__( payjson, __KF_STRING__( payorder ), kfpaydata->_value[ __KF_STRING__( payorder ) ] );
                        __JSON_SET_VALUE__( payjson, __KF_STRING__( payid ), kfpaydata->_value[ __KF_STRING__( payid ) ] );

                        __JSON_ADD_VALUE__( payarray, payjson );

                        // 更新状态
                        auto updataresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( pay ), order, __KF_STRING__( flag ), 1 );
                        if ( !updataresult->IsOk() )
                        {
                            __LOG_ERROR__( "update player=[{}] pay=[{}] flag failed!", playerid, order );
                        }
                    }
                }
            }

            // 如果不为空, 返回给玩家
            auto size = __JSON_ARRAY_SIZE__( payarray );
            if ( size > 0u )
            {
                __JSON_SET_VALUE__( response, __KF_STRING__( paydata ), payarray );
            }
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleFinishPay )
    {
        __JSON_PARSE_STRING__( request, data );
        auto order = __JSON_GET_STRING__( request, __KF_STRING__( payorder ) );
        auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );

        // 删除充值信息
        auto redisdriver = __AUTH_REDIS_DRIVER__;
        redisdriver->Append( "del {}:{}", __KF_STRING__( pay ), order );
        redisdriver->Append( "srem {}:{} {}", __KF_STRING__( paydata ), playerid, order );
        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_WARN__( "player=[{}] finish pay=[{}] failed!", playerid, order );
        }

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleInternalPay )
    {
        __LOG_INFO__( "channel=[{}] pay=[{}]", KFMsg::Internal, data );
        return _kf_channel->AuthPay( KFMsg::Internal, data );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
}