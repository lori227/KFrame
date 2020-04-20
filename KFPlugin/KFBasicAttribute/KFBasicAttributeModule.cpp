#include "KFBasicAttributeModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBasicAttributeModule::BeforeRun()
    {
        auto authdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( basicdatabase ) );
        switch ( authdatabasetype )
        {
        case KFMsg::Mongo:
            _basic_attribute_logic = __NEW_OBJECT__( KFBasicAttributeMongo );
            break;
        default:
            _basic_attribute_logic = __NEW_OBJECT__( KFBasicAttributeRedis );
            break;
        }
    }

    void KFBasicAttributeModule::BeforeShut()
    {
        __DELETE_OBJECT__( _basic_attribute_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFBasicAttributeModule::UpdateBasicIntValue( uint64 playerid, uint64 serverid, const KeyValue& values )
    {
        _basic_attribute_logic->UpdateBasicIntValue( playerid, serverid, values );
    }

    void KFBasicAttributeModule::UpdateBasicStrValue( uint64 playerid, uint64 serverid, const StringMap& values )
    {
        _basic_attribute_logic->UpdateBasicStrValue( playerid, serverid, values );
    }

    uint64 KFBasicAttributeModule::QueryBasicPlayerid( const std::string& playername, uint32 zoneid )
    {
        return _basic_attribute_logic->QueryBasicPlayerid( playername, zoneid );
    }

    void KFBasicAttributeModule::ClearBasicServerId( uint64 serverid )
    {
        _basic_attribute_logic->ClearBasicServerId( serverid );
    }

    uint64 KFBasicAttributeModule::QueryBasicServerId( uint64 playerid )
    {
        return _basic_attribute_logic->QueryBasicServerId( playerid );
    }

    uint32 KFBasicAttributeModule::QueryBasicAttribute( uint64 playerid, StringMap& values )
    {
        return _basic_attribute_logic->QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicAttributeModule::QueryBasicAttribute( const std::string& playername, uint32 zoneid, StringMap& values )
    {
        auto playerid = QueryBasicPlayerid( playername, zoneid );
        if ( playerid == 0u )
        {
            return KFMsg::QueryBasicNotExist;
        }

        return QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicAttributeModule::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        return _basic_attribute_logic->SetPlayerName( zoneid, playerid, oldname, newname );
    }
}