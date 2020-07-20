#include "KFDirDatabaseModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDirDatabaseModule::BeforeRun()
    {
        auto dirdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( dirdatabase ) );
        switch ( dirdatabasetype )
        {
        case KFMsg::Mongo:
            _dir_attribute_logic = __NEW_OBJECT__( KFDirDatabaseMongo );
            break;
        default:
            _dir_attribute_logic = __NEW_OBJECT__( KFDirDatabaseRedis );
            break;
        }

        srand( KFGlobal::Instance()->_real_time );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirDatabaseModule::BeforeShut()
    {
        __DELETE_OBJECT__( _dir_attribute_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDirDatabaseModule::ZoneRegister( KFJson& zonedata )
    {
        auto zoneid = __JSON_GET_UINT32__( zonedata, __STRING__( zoneid ) );
        return _dir_attribute_logic->ZoneRegister( zoneid, zonedata );
    }

    bool KFDirDatabaseModule::ZoneUpdate( KFJson& zonedata )
    {
        auto appid = __JSON_GET_UINT64__( zonedata, __STRING__( appid ) );
        auto zoneid = __JSON_GET_UINT32__( zonedata, __STRING__( loginzoneid ) );
        auto count = __JSON_GET_UINT32__( zonedata, __STRING__( count ) );
        auto ip = __JSON_GET_STRING__( zonedata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( zonedata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( zonedata, __STRING__( time ) );
        return _dir_attribute_logic->ZoneUpdate( appid, zoneid, count, ip, port, time );
    }

    StringMapList KFDirDatabaseModule::QueryZoneList( const std::string& flag )
    {
        return _dir_attribute_logic->QueryZoneList( flag );
    }

    StringMap KFDirDatabaseModule::QueryZoneIp( uint32 zoneid )
    {
        return _dir_attribute_logic->QueryZoneIp( zoneid );
    }

    bool KFDirDatabaseModule::ZoneBalance( uint32 zoneid, uint32 count )
    {
        return _dir_attribute_logic->ZoneBalance( zoneid, count );
    }

    bool KFDirDatabaseModule::SetZoneRecommend( const std::string& flag, uint32 zoneid, bool isrecommend )
    {
        return _dir_attribute_logic->SetZoneRecommend( flag, zoneid, isrecommend );
    }

    StringMap KFDirDatabaseModule::AllocPlayerZone( const std::string& flag, uint32 zoneid )
    {
        return _dir_attribute_logic->AllocPlayerZone( flag, zoneid );
    }

    uint32 KFDirDatabaseModule::QueryZoneStatus( uint32 zoneid )
    {
        return _dir_attribute_logic->QueryZoneStatus( zoneid );
    }

    bool KFDirDatabaseModule::UpdateZoneStatus( uint32 zoneid, uint32 status )
    {
        return _dir_attribute_logic->UpdateZoneStatus( zoneid, status );
    }

    bool KFDirDatabaseModule::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        return _dir_attribute_logic->SetWorldUrl( worldid, url );
    }

    std::string KFDirDatabaseModule::GetWorldUrl( uint64 worldid )
    {
        return _dir_attribute_logic->GetWorldUrl( worldid );
    }

    bool KFDirDatabaseModule::UpdateMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto appid = __JSON_GET_UINT64__( masterdata, __STRING__( appid ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        auto ip = __JSON_GET_STRING__( masterdata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( masterdata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( masterdata, __STRING__( time ) );
        return _dir_attribute_logic->UpdateMasterIp( appname, appid, zoneid, ip, port, time );
    }

    StringMap KFDirDatabaseModule::QueryMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_attribute_logic->QueryMasterIp( appname, zoneid );
    }

    StringMapList KFDirDatabaseModule::QueryMasterList( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_attribute_logic->QueryMasterList( appname, zoneid );
    }
}