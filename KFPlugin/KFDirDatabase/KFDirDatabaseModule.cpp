#include "KFDirDatabaseModule.hpp"

namespace KFrame
{
    void KFDirDatabaseModule::BeforeRun()
    {
        auto dir_database_type = KFGlobal::Instance()->GetUInt32( __STRING__( dirdatabase ) );
        switch ( dir_database_type )
        {
        case KFDatabaseEnum::Mongo:
            _dir_database_logic = __NEW_OBJECT__( KFDirDatabaseMongo );
            break;
        default:
            _dir_database_logic = __NEW_OBJECT__( KFDirDatabaseRedis );
            break;
        }

        srand( KFGlobal::Instance()->_real_time );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirDatabaseModule::BeforeShut()
    {
        __DELETE_OBJECT__( _dir_database_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDirDatabaseModule::ZoneRegister( KFJson& zone_data )
    {
        auto zone_id = __JSON_GET_UINT32__( zone_data, __STRING__( zoneid ) );
        return _dir_database_logic->ZoneRegister( zone_id, zone_data );
    }

    bool KFDirDatabaseModule::ZoneUpdate( KFJson& zone_data )
    {
        auto app_id = __JSON_GET_UINT64__( zone_data, __STRING__( appid ) );
        auto zone_id = __JSON_GET_UINT32__( zone_data, __STRING__( loginzoneid ) );
        auto count = __JSON_GET_UINT32__( zone_data, __STRING__( count ) );
        auto ip = __JSON_GET_STRING__( zone_data, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( zone_data, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( zone_data, __STRING__( time ) );
        return _dir_database_logic->ZoneUpdate( app_id, zone_id, count, ip, port, time );
    }

    StringMapList KFDirDatabaseModule::QueryZoneList( const std::string& flag )
    {
        return _dir_database_logic->QueryZoneList( flag );
    }

    StringMap KFDirDatabaseModule::QueryZoneIp( uint32 zone_id )
    {
        return _dir_database_logic->QueryZoneIp( zone_id );
    }

    bool KFDirDatabaseModule::ZoneBalance( uint32 zone_id, uint32 count )
    {
        return _dir_database_logic->ZoneBalance( zone_id, count );
    }

    bool KFDirDatabaseModule::SetZoneRecommend( const std::string& flag, uint32 zone_id, bool is_recommend )
    {
        return _dir_database_logic->SetZoneRecommend( flag, zone_id, is_recommend );
    }

    StringMap KFDirDatabaseModule::AllocPlayerZone( const std::string& flag, uint32 zone_id )
    {
        return _dir_database_logic->AllocPlayerZone( flag, zone_id );
    }

    uint32 KFDirDatabaseModule::QueryZoneStatus( uint32 zone_id )
    {
        return _dir_database_logic->QueryZoneStatus( zone_id );
    }

    bool KFDirDatabaseModule::UpdateZoneStatus( uint32 zone_id, uint32 status )
    {
        return _dir_database_logic->UpdateZoneStatus( zone_id, status );
    }

    bool KFDirDatabaseModule::SetWorldUrl( uint64 world_id, const std::string& url )
    {
        return _dir_database_logic->SetWorldUrl( world_id, url );
    }

    std::string KFDirDatabaseModule::GetWorldUrl( uint64 world_id )
    {
        return _dir_database_logic->GetWorldUrl( world_id );
    }

    bool KFDirDatabaseModule::UpdateMasterIp( KFJson& master_data )
    {
        auto app_name = __JSON_GET_STRING__( master_data, __STRING__( appname ) );
        auto app_id = __JSON_GET_UINT64__( master_data, __STRING__( appid ) );
        auto zone_id = __JSON_GET_UINT32__( master_data, __STRING__( zoneid ) );
        auto ip = __JSON_GET_STRING__( master_data, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( master_data, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( master_data, __STRING__( time ) );
        return _dir_database_logic->UpdateMasterIp( app_name, app_id, zone_id, ip, port, time );
    }

    StringMap KFDirDatabaseModule::QueryMasterIp( KFJson& master_data )
    {
        auto app_name = __JSON_GET_STRING__( master_data, __STRING__( appname ) );
        auto zone_id = __JSON_GET_UINT32__( master_data, __STRING__( zoneid ) );
        return _dir_database_logic->QueryMasterIp( app_name, zone_id );
    }

    StringMapList KFDirDatabaseModule::QueryMasterList( KFJson& master_data )
    {
        auto app_name = __JSON_GET_STRING__( master_data, __STRING__( appname ) );
        auto zone_id = __JSON_GET_UINT32__( master_data, __STRING__( zoneid ) );
        return _dir_database_logic->QueryMasterList( app_name, zone_id );
    }
}