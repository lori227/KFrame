#include "KFDirShardModule.hpp"

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
        __REGISTER_HTTP__( __STRING__( zonestatus ), false, &KFDirShardModule::HandleZoneStatus );

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
        __UN_HTTP__( __STRING__( zonestatus ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto ok = _kf_dir_database->ZoneRegister( request );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneUpdate )
    {
        __JSON_PARSE_STRING__( request, data );

        auto ok = _kf_dir_database->ZoneUpdate( request );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneStatus )
    {
        __JSON_PARSE_STRING__( request, data );

        auto zone_id = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto status = __JSON_GET_UINT32__( request, __STRING__( status ) );
        auto ok = _kf_dir_database->UpdateZoneStatus( zone_id, status );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryZoneList )
    {
        __JSON_PARSE_STRING__( request, data );

        auto flag = __JSON_GET_STRING__( request, __STRING__( flag ) );
        auto zone_data_list = _kf_dir_database->QueryZoneList( flag );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( array_data );
        for ( auto& zone_data : zone_data_list )
        {
            __JSON_OBJECT__( kfzone );
            __JSON_SET_VALUE__( kfzone, __STRING__( zoneid ), __TO_UINT32__( zone_data[ __STRING__( zoneid ) ] ) );
            __JSON_SET_VALUE__( kfzone, __STRING__( name ), zone_data[ __STRING__( name ) ] );
            __JSON_SET_VALUE__( kfzone, __STRING__( ip ), zone_data[ __STRING__( ip ) ] );
            __JSON_SET_VALUE__( kfzone, __STRING__( port ), __TO_UINT32__( zone_data[ __STRING__( port ) ] ) );
            __JSON_ADD_VALUE__( array_data, kfzone );
        }
        __JSON_SET_VALUE__( response, __STRING__( zonelist ), array_data );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryZoneIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zone_id = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto zone_data = _kf_dir_database->QueryZoneIp( zone_id );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( zoneid ), zone_id );
        __JSON_SET_VALUE__( response, __STRING__( ip ), zone_data[ __STRING__( ip ) ] );
        __JSON_SET_VALUE__( response, __STRING__( port ), __TO_UINT32__( zone_data[ __STRING__( port ) ] ) );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneBalance )
    {
        __JSON_PARSE_STRING__( request, data );
        auto zone_id = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto count = __JSON_GET_UINT64__( request, __STRING__( count ) );

        auto ok = _kf_dir_database->ZoneBalance( zone_id, count );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleZoneRecommend )
    {
        __JSON_PARSE_STRING__( request, data );
        auto flag = __JSON_GET_STRING__( request, __STRING__( flag ) );
        auto zone_id = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto recommend = __JSON_GET_UINT32__( request, __STRING__( recommend ) );
        auto ok = _kf_dir_database->SetZoneRecommend( flag, zone_id, recommend == 1u );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleWorldRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto world_id = __JSON_GET_UINT64__( request, __STRING__( world ) );
        auto url = __JSON_GET_STRING__( request, __STRING__( url ) );

        auto ok = _kf_dir_database->SetWorldUrl( world_id, url );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleUpdateMasterIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto ok = _kf_dir_database->UpdateMasterIp( request );
        return _kf_http_server->SendCode( ok ? KFEnum::Ok : KFEnum::Error );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryMasterIp )
    {
        __JSON_PARSE_STRING__( request, data );
        auto master_data = _kf_dir_database->QueryMasterIp( request );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_FROM_MAP__( response, master_data );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryMasterList )
    {
        __JSON_PARSE_STRING__( request, data );
        auto master_data_list = _kf_dir_database->QueryMasterList( request );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( array_data );
        for ( auto& master_data : master_data_list )
        {
            __JSON_OBJECT__( masterjson );
            __JSON_FROM_MAP__( masterjson, master_data );
            __JSON_ADD_VALUE__( array_data, masterjson );
        }
        __JSON_SET_VALUE__( response, __STRING__( masterlist ), array_data );
        return _kf_http_server->SendResponse( response );
    }
}