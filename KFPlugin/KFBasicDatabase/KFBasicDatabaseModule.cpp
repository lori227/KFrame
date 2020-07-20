#include "KFBasicDatabaseModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBasicDatabaseModule::BeforeRun()
    {
        auto databasetype = KFGlobal::Instance()->GetUInt32( __STRING__( basicdatabase ) );
        switch ( databasetype )
        {
        case KFMsg::Mongo:
            _basic_attribute_logic = __NEW_OBJECT__( KFBasicDatabaseMongo );
            break;
        default:
            _basic_attribute_logic = __NEW_OBJECT__( KFBasicDatabaseRedis );
            break;
        }
    }

    void KFBasicDatabaseModule::BeforeShut()
    {
        __DELETE_OBJECT__( _basic_attribute_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFBasicDatabaseModule::UpdateBasicIntValue( uint64 playerid, uint64 serverid, const StringUInt64& values )
    {
        _basic_attribute_logic->UpdateBasicIntValue( playerid, serverid, values );
    }

    void KFBasicDatabaseModule::UpdateBasicStrValue( uint64 playerid, uint64 serverid, const StringMap& values )
    {
        _basic_attribute_logic->UpdateBasicStrValue( playerid, serverid, values );
    }

    uint64 KFBasicDatabaseModule::QueryBasicPlayerid( const std::string& playername, uint32 zoneid )
    {
        return _basic_attribute_logic->QueryBasicPlayerid( playername, zoneid );
    }

    void KFBasicDatabaseModule::ClearBasicServerId( uint64 serverid )
    {
        _basic_attribute_logic->ClearBasicServerId( serverid );
    }

    uint64 KFBasicDatabaseModule::QueryBasicServerId( uint64 playerid )
    {
        return _basic_attribute_logic->QueryBasicServerId( playerid );
    }

    uint32 KFBasicDatabaseModule::QueryBasicAttribute( uint64 playerid, StringMap& values )
    {
        return _basic_attribute_logic->QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicDatabaseModule::QueryBasicAttribute( const std::string& playername, uint32 zoneid, StringMap& values )
    {
        auto playerid = QueryBasicPlayerid( playername, zoneid );
        if ( playerid == 0u )
        {
            return KFMsg::QueryBasicNotExist;
        }

        return QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicDatabaseModule::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        return _basic_attribute_logic->SetPlayerName( zoneid, playerid, oldname, newname );
    }

    uint64 KFBasicDatabaseModule::QueryBasicIntValue( uint64 playerid, const std::string& dataname )
    {
        return _basic_attribute_logic->QueryBasicIntValue( playerid, dataname );
    }

    std::string KFBasicDatabaseModule::QueryBasicStrValue( uint64 playerid, const std::string& dataname )
    {
        return _basic_attribute_logic->QueryBasicStrValue( playerid, dataname );
    }
}