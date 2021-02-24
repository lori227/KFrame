#include "KFRelationDatabaseModule.hpp"

namespace KFrame
{
    void KFRelationDatabaseModule::BeforeRun()
    {
        auto databasetype = KFGlobal::Instance()->GetUInt32( __STRING__( relationdatabase ) );
        switch ( databasetype )
        {
        case KFDatabaseEnum::Mongo:
            _relation_database_logic = __NEW_OBJECT__( KFRelationDatabaseMongo );
            break;
        default:
            _relation_database_logic = __NEW_OBJECT__( KFRelationDatabaseRedis );
            break;
        }
    }

    void KFRelationDatabaseModule::BeforeShut()
    {
        __DELETE_OBJECT__( _relation_database_logic );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationDatabaseModule::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        return _relation_database_logic->QueryRelationList( listname, relationname, player_id, relationlist );
    }

    void KFRelationDatabaseModule::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        return _relation_database_logic->QueryInviteList( listname, relationname, player_id, relationlist );
    }

    bool KFRelationDatabaseModule::RelationExist( const std::string& listname, uint64 player_id, uint64 targetid )
    {
        return _relation_database_logic->RelationExist( listname, player_id, targetid );
    }

    uint32 KFRelationDatabaseModule::RelationCount( const std::string& listname, uint64 player_id )
    {
        return _relation_database_logic->RelationCount( listname, player_id );
    }

    void KFRelationDatabaseModule::AddRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        return _relation_database_logic->AddRelation( listname, relationname, player_id, targetid, bothway );
    }

    void KFRelationDatabaseModule::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        return _relation_database_logic->RemoveRelation( listname, relationname, player_id, targetid, bothway );
    }

    void KFRelationDatabaseModule::AddInvite( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        return _relation_database_logic->AddInvite( listname, relationname, player_id, targetid, message, keeptime );
    }

    bool KFRelationDatabaseModule::IsRefuse( const std::string& refusename, uint64 player_id )
    {
        return _relation_database_logic->IsRefuse( refusename, player_id );
    }

    void KFRelationDatabaseModule::SetRefuse( const std::string& refusename, uint64 player_id, uint32 refusevalue )
    {
        return _relation_database_logic->SetRefuse( refusename, player_id, refusevalue );
    }
}