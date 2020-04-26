#include "KFDirAttributeModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDirAttributeModule::BeforeRun()
    {
        auto dirdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( dirdatabase ) );
        switch ( dirdatabasetype )
        {
        case KFMsg::Mongo:
            _dir_attribute_logic = __NEW_OBJECT__( KFDirAttributeMongo );
            break;
        default:
            _dir_attribute_logic = __NEW_OBJECT__( KFDirAttributeRedis );
            break;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirAttributeModule::BeforeShut()
    {
        __DELETE_OBJECT__( _dir_attribute_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDirAttributeModule::ZoneRegister( KFJson& zonedata )
    {
        auto zoneid = __JSON_GET_UINT32__( zonedata, __STRING__( zoneid ) );
        return _dir_attribute_logic->ZoneRegister( zoneid, zonedata );
    }

    bool KFDirAttributeModule::ZoneUpdate( KFJson& zonedata )
    {
        auto appid = __JSON_GET_UINT64__( zonedata, __STRING__( appid ) );
        auto zoneid = __JSON_GET_UINT32__( zonedata, __STRING__( zoneid ) );
        auto count = __JSON_GET_UINT32__( zonedata, __STRING__( count ) );
        auto ip = __JSON_GET_STRING__( zonedata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( zonedata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( zonedata, __STRING__( time ) );
        return _dir_attribute_logic->ZoneUpdate( appid, zoneid, count, ip, port, time );
    }

    StringListMap KFDirAttributeModule::QueryZoneList()
    {
        return _dir_attribute_logic->QueryZoneList();
    }

    StringMap KFDirAttributeModule::QueryZoneIp( uint32 zoneid )
    {
        return _dir_attribute_logic->QueryZoneIp( zoneid );
    }

    bool KFDirAttributeModule::ZoneBalance( uint32 zoneid, uint32 count )
    {
        return _dir_attribute_logic->ZoneBalance( zoneid, count );
    }

    bool KFDirAttributeModule::SetZoneRecommend( uint32 zoneid )
    {
        return _dir_attribute_logic->SetZoneRecommend( zoneid );
    }

    StringMap KFDirAttributeModule::AllocPlayerZone( uint32 zoneid )
    {
        return _dir_attribute_logic->AllocPlayerZone( zoneid );
    }

    bool KFDirAttributeModule::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        return _dir_attribute_logic->SetWorldUrl( worldid, url );
    }

    std::string KFDirAttributeModule::GetWorldUrl( uint64 worldid )
    {
        return _dir_attribute_logic->GetWorldUrl( worldid );
    }

    bool KFDirAttributeModule::UpdateMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto appid = __JSON_GET_UINT64__( masterdata, __STRING__( appid ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        auto ip = __JSON_GET_STRING__( masterdata, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( masterdata, __STRING__( port ) );
        auto time = __JSON_GET_UINT32__( masterdata, __STRING__( time ) );
        return _dir_attribute_logic->UpdateMasterIp( appname, appid, zoneid, ip, port, time );
    }

    StringMap KFDirAttributeModule::QueryMasterIp( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_attribute_logic->QueryMasterIp( appname, zoneid );
    }

    StringListMap KFDirAttributeModule::QueryMasterList( KFJson& masterdata )
    {
        auto appname = __JSON_GET_STRING__( masterdata, __STRING__( appname ) );
        auto zoneid = __JSON_GET_UINT32__( masterdata, __STRING__( zoneid ) );
        return _dir_attribute_logic->QueryMasterList( appname, zoneid );
    }
}