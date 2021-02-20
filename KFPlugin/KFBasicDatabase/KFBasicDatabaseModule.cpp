#include "KFBasicDatabaseModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBasicDatabaseModule::BeforeRun()
    {
        auto databasetype = KFGlobal::Instance()->GetUInt32( __STRING__( basicdatabase ) );
        switch ( databasetype )
        {
        case KFDatabaseEnum::Mongo:
            _basic_database_logic = __NEW_OBJECT__( KFBasicDatabaseMongo );
            break;
        default:
            _basic_database_logic = __NEW_OBJECT__( KFBasicDatabaseRedis );
            break;
        }
    }

    void KFBasicDatabaseModule::BeforeShut()
    {
        __DELETE_OBJECT__( _basic_database_logic );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFBasicDatabaseModule::UpdateBasicIntValue( uint64 playerid, uint64 server_id, const StringUInt64& values )
    {
        _basic_database_logic->UpdateBasicIntValue( playerid, server_id, values );
    }

    void KFBasicDatabaseModule::UpdateBasicStrValue( uint64 playerid, uint64 server_id, const StringMap& values )
    {
        _basic_database_logic->UpdateBasicStrValue( playerid, server_id, values );
    }

    uint64 KFBasicDatabaseModule::QueryBasicPlayerid( const std::string& playername, uint32 zone_id )
    {
        return _basic_database_logic->QueryBasicPlayerid( playername, zone_id );
    }

    void KFBasicDatabaseModule::ClearBasicServerId( uint64 server_id )
    {
        _basic_database_logic->ClearBasicServerId( server_id );
    }

    uint64 KFBasicDatabaseModule::QueryBasicServerId( uint64 playerid )
    {
        return _basic_database_logic->QueryBasicServerId( playerid );
    }

    uint32 KFBasicDatabaseModule::QueryBasicAttribute( uint64 playerid, StringMap& values )
    {
        return _basic_database_logic->QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicDatabaseModule::QueryBasicAttribute( const std::string& playername, uint32 zone_id, StringMap& values )
    {
        auto playerid = QueryBasicPlayerid( playername, zone_id );
        if ( playerid == 0u )
        {
            return KFMsg::QueryBasicNotExist;
        }

        return QueryBasicAttribute( playerid, values );
    }

    uint32 KFBasicDatabaseModule::SetPlayerName( uint32 zone_id, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        return _basic_database_logic->SetPlayerName( zone_id, playerid, oldname, newname );
    }

    uint64 KFBasicDatabaseModule::QueryBasicIntValue( uint64 playerid, const std::string& data_name )
    {
        return _basic_database_logic->QueryBasicIntValue( playerid, data_name );
    }

    std::string KFBasicDatabaseModule::QueryBasicStrValue( uint64 playerid, const std::string& data_name )
    {
        return _basic_database_logic->QueryBasicStrValue( playerid, data_name );
    }
}