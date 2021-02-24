#include "KFDirDatabaseModule.hpp"

namespace KFrame
{
    void KFDirDatabaseModule::BeforeRun()
    {
        auto dirdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( dirdatabase ) );
        switch ( dirdatabasetype )
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
    bool KFDirDatabaseModule::ZoneRegister( KFJson& zonedata )
    {
        auto zone_id = __JSON_GET_UINT32__( zonedata, __STRING__( zoneid ) );
        return _dir_database_logic->ZoneRegister( zone_id, zonedata );
    }

    bool KFDirDatabaseModule::ZoneUpdate( KFJson& zonedata )
    {
        auto appid = __JSON_GET_UINT64__( zonedata, __STRING__( appid ) );
        auto zone_id = __JSON_GET_UINT32__( zonedata, __STRING__( loginzoneid ) );
        auto count = __JSON_GET_UINT32__( zonedata, __STRING__( count ) );
        auto ip = __JSON_GET_STRING__( zonedata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( zonedata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( zonedata, __STRING__( time ) );
        return _dir_database_logic->ZoneUpdate( appid, zone_id, count, ip, port, time );
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

    bool KFDirDatabaseModule::SetZoneRecommend( const std::string& flag, uint32 zone_id, bool isrecommend )
    {
        return _dir_database_logic->SetZoneRecommend( flag, zone_id, isrecommend );
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

    bool KFDirDatabaseModule::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        return _dir_database_logic->SetWorldUrl( worldid, url );
    }

    std::string KFDirDatabaseModule::GetWorldUrl( uint64 worldid )
    {
        return _dir_database_logic->GetWorldUrl( worldid );
    }

    bool KFDirDatabaseModule::UpdateMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto appid = __JSON_GET_UINT64__( masterdata, __STRING__( appid ) );
        auto zone_id = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        auto ip = __JSON_GET_STRING__( masterdata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( masterdata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( masterdata, __STRING__( time ) );
        return _dir_database_logic->UpdateMasterIp( appname, appid, zone_id, ip, port, time );
    }

    StringMap KFDirDatabaseModule::QueryMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zone_id = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_database_logic->QueryMasterIp( appname, zone_id );
    }

    StringMapList KFDirDatabaseModule::QueryMasterList( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zone_id = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_database_logic->QueryMasterList( appname, zone_id );
    }
}