#include "KFDirShardModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDirShardModule::BeforeRun()
    {
        auto dirdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( dirdatabase ) );
        switch ( dirdatabasetype )
        {
        case KFMsg::Mongo:
            _dir_shard_logic = __NEW_OBJECT__( KFDirShardMongo );
            break;
        default:
            _dir_shard_logic = __NEW_OBJECT__( KFDirShardRedis );
            break;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( zoneregister ), false, &KFDirShardModule::HandleZoneRegister );
        __REGISTER_HTTP__( __STRING__( zoneupdate ), false, &KFDirShardModule::HandleZoneUpdate );
        __REGISTER_HTTP__( __STRING__( zonelist ), false, &KFDirShardModule::HandleQueryZoneList );
        __REGISTER_HTTP__( __STRING__( zoneip ), false, &KFDirShardModule::HandleQueryZoneIp );
        __REGISTER_HTTP__( __STRING__( zonebalance ), false, &KFDirShardModule::HandleZoneBalance );
        __REGISTER_HTTP__( __STRING__( zonerecommend ), false, &KFDirShardModule::HandleZoneRecommend );
        __REGISTER_HTTP__( __STRING__( worldregister ), false, &KFDirShardModule::HandleWorldRegister );

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirShardModule::BeforeShut()
    {
        __DELETE_OBJECT__( _dir_shard_logic );
        /////////////////////////////////////////////////////////////////////////////////////////
        __UN_HTTP__( __STRING__( zoneregister ) );
        __UN_HTTP__( __STRING__( zoneupdate ) );
        __UN_HTTP__( __STRING__( zonelist ) );
        __UN_HTTP__( __STRING__( zoneip ) );
        __UN_HTTP__( __STRING__( zonebalance ) );
        __UN_HTTP__( __STRING__( zonerecommend ) );
        __UN_HTTP__( __STRING__( worldregister ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto ok = _dir_shard_logic->ZoneRegister( zoneid, request );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneUpdate )
    {
        __JSON_PARSE_STRING__( request, data );

        auto appid = __JSON_GET_UINT64__( request, __STRING__( appid ) );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto count = __JSON_GET_UINT32__( request, __STRING__( count ) );
        auto gateip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( request, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( request, __STRING__( time ) );
        auto ok = _dir_shard_logic->ZoneUpdate( appid, zoneid, count, gateip, port, time );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryZoneList )
    {
        auto zonedatalist = _dir_shard_logic->QueryZoneList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( kfarray );
        for ( auto& zonedata : zonedatalist )
        {
            __JSON_OBJECT__( kfzone );
            __JSON_SET_VALUE__( kfzone, __STRING__( zoneid ), __TO_UINT32__( zonedata[ __STRING__( zoneid ) ] ) );
            __JSON_SET_VALUE__( kfzone, __STRING__( name ), zonedata[ __STRING__( name ) ] );
            __JSON_SET_VALUE__( kfzone, __STRING__( ip ), zonedata[ __STRING__( ip ) ] );
            __JSON_SET_VALUE__( kfzone, __STRING__( port ), __TO_UINT32__( zonedata[ __STRING__( port ) ] ) );
            __JSON_ADD_VALUE__( kfarray, kfzone );
        }
        __JSON_SET_VALUE__( response, __STRING__( zonelist ), kfarray );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryZoneIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto zonedata = _dir_shard_logic->QueryZoneIp( zoneid );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( zoneid ), zoneid );
        __JSON_SET_VALUE__( response, __STRING__( ip ), zonedata[ __STRING__( ip ) ] );
        __JSON_SET_VALUE__( response, __STRING__( port ), __TO_UINT32__( zonedata[ __STRING__( port ) ] ) );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneBalance )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto count = __JSON_GET_UINT64__( request, __STRING__( count ) );

        auto ok = _dir_shard_logic->ZoneBalance( zoneid, count );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    bool KFDirShardModule::ZoneBalance( uint32 zoneid, uint32 count )
    {
        return _dir_shard_logic->ZoneBalance( zoneid, count );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRecommend )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto ok = _dir_shard_logic->SetZoneRecommend( zoneid );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleWorldRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto worldid = __JSON_GET_UINT64__( request, __STRING__( world ) );
        auto url = __JSON_GET_STRING__( request, __STRING__( url ) );

        auto ok = _dir_shard_logic->SetWorldUrl( worldid, url );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    StringMap KFDirShardModule::AllocPlayerZone( uint32 zoneid )
    {
        return _dir_shard_logic->AllocPlayerZone( zoneid );
    }

    std::string KFDirShardModule::GetWorldUrl( uint64 worldid )
    {
        return _dir_shard_logic->GetWorldUrl( worldid );
    }
}