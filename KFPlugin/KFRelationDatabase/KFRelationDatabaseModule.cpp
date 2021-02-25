#include "KFRelationDatabaseModule.hpp"

namespace KFrame
{
    void KFRelationDatabaseModule::BeforeRun()
    {
        auto database_type = KFGlobal::Instance()->GetUInt32( __STRING__( relationdatabase ) );
        switch ( database_type )
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
    void KFRelationDatabaseModule::QueryRelationList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        return _relation_database_logic->QueryRelationList( list_name, relation_name, player_id, relation_list );
    }

    void KFRelationDatabaseModule::QueryInviteList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        return _relation_database_logic->QueryInviteList( list_name, relation_name, player_id, relation_list );
    }

    bool KFRelationDatabaseModule::RelationExist( const std::string& list_name, uint64 player_id, uint64 target_id )
    {
        return _relation_database_logic->RelationExist( list_name, player_id, target_id );
    }

    uint32 KFRelationDatabaseModule::RelationCount( const std::string& list_name, uint64 player_id )
    {
        return _relation_database_logic->RelationCount( list_name, player_id );
    }

    void KFRelationDatabaseModule::AddRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        return _relation_database_logic->AddRelation( list_name, relation_name, player_id, target_id, is_both_way );
    }

    void KFRelationDatabaseModule::RemoveRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        return _relation_database_logic->RemoveRelation( list_name, relation_name, player_id, target_id, is_both_way );
    }

    void KFRelationDatabaseModule::AddInvite( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, const std::string& message, uint64 keep_time )
    {
        return _relation_database_logic->AddInvite( list_name, relation_name, player_id, target_id, message, keep_time );
    }

    bool KFRelationDatabaseModule::IsRefuse( const std::string& refuse_name, uint64 player_id )
    {
        return _relation_database_logic->IsRefuse( refuse_name, player_id );
    }

    void KFRelationDatabaseModule::SetRefuse( const std::string& refuse_name, uint64 player_id, uint32 refuse_value )
    {
        return _relation_database_logic->SetRefuse( refuse_name, player_id, refuse_value );
    }
}