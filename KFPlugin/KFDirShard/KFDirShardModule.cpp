#include "KFDirShardModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDirShardModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( zoneregister ), false, &KFDirShardModule::HandleZoneRegister );
        __REGISTER_HTTP__( __STRING__( zoneupdate ), false, &KFDirShardModule::HandleZoneUpdate );
        __REGISTER_HTTP__( __STRING__( zonelist ), false, &KFDirShardModule::HandleQueryZoneList );
        __REGISTER_HTTP__( __STRING__( zoneip ), false, &KFDirShardModule::HandleQueryZoneIp );
        __REGISTER_HTTP__( __STRING__( zonebalance ), false, &KFDirShardModule::HandleZoneBalance );
        __REGISTER_HTTP__( __STRING__( zonerecommend ), false, &KFDirShardModule::HandleZoneRecommend );
        __REGISTER_HTTP__( __STRING__( worldregister ), false, &KFDirShardModule::HandleWorldRegister );
        __REGISTER_HTTP__( __STRING__( updatemasterip ), false, &KFDirShardModule::HandleUpdateMasterIp );
        __REGISTER_HTTP__( __STRING__( querymasterip ), false, &KFDirShardModule::HandleQueryMasterIp );
        __REGISTER_HTTP__( __STRING__( querymasterlist ), false, &KFDirShardModule::HandleQueryMasterList );

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirShardModule::BeforeShut()
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        __UN_HTTP__( __STRING__( zoneregister ) );
        __UN_HTTP__( __STRING__( zoneupdate ) );
        __UN_HTTP__( __STRING__( zonelist ) );
        __UN_HTTP__( __STRING__( zoneip ) );
        __UN_HTTP__( __STRING__( zonebalance ) );
        __UN_HTTP__( __STRING__( zonerecommend ) );
        __UN_HTTP__( __STRING__( worldregister ) );
        __UN_HTTP__( __STRING__( updatemasterip ) );
        __UN_HTTP__( __STRING__( querymasterip ) );
        __UN_HTTP__( __STRING__( querymasterlist ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto ok = _kf_dir_attribute->ZoneRegister( request );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneUpdate )
    {
        __JSON_PARSE_STRING__( request, data );

        auto ok = _kf_dir_attribute->ZoneUpdate( request );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryZoneList )
    {
        auto zonedatalist = _kf_dir_attribute->QueryZoneList();

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
        auto zonedata = _kf_dir_attribute->QueryZoneIp( zoneid );

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

        auto ok = _kf_dir_attribute->ZoneBalance( zoneid, count );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRecommend )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto ok = _kf_dir_attribute->SetZoneRecommend( zoneid );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleWorldRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto worldid = __JSON_GET_UINT64__( request, __STRING__( world ) );
        auto url = __JSON_GET_STRING__( request, __STRING__( url ) );

        auto ok = _kf_dir_attribute->SetWorldUrl( worldid, url );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleUpdateMasterIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto ok = _kf_dir_attribute->UpdateMasterIp( request );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryMasterIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto masterdata = _kf_dir_attribute->QueryMasterIp( request );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_FROM_MAP__( response, masterdata );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryMasterList )
    {
        __JSON_PARSE_STRING__( request, data );
        auto masterdatalist = _kf_dir_attribute->QueryMasterList( request );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( kfarray );
        for ( auto& masterdata : masterdatalist )
        {
            __JSON_OBJECT__( masterjson );
            __JSON_FROM_MAP__( masterjson, masterdata );
            __JSON_ADD_VALUE__( kfarray, masterjson );
        }
        __JSON_SET_VALUE__( response, __STRING__( masterlist ), kfarray );
        return _kf_http_server->SendResponse( response );
    }
}