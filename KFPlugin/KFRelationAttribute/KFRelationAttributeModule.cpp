#include "KFRelationAttributeModule.hpp"

namespace KFrame
{
    void KFRelationAttributeModule::BeforeRun()
    {
        auto databasetype = KFGlobal::Instance()->GetUInt32( __STRING__( relationdatabase ) );
        switch ( databasetype )
        {
        case KFMsg::Mongo:
            _relation_attribute_logic = __NEW_OBJECT__( KFRelationAttributeMongo );
            break;
        default:
            _relation_attribute_logic = __NEW_OBJECT__( KFRelationAttributeRedis );
            break;
        }
    }

    void KFRelationAttributeModule::BeforeShut()
    {
        __DELETE_OBJECT__( _relation_attribute_logic );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationAttributeModule::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        return _relation_attribute_logic->QueryRelationList( listname, relationname, playerid, relationlist );
    }

    void KFRelationAttributeModule::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        return _relation_attribute_logic->QueryInviteList( listname, relationname, playerid, relationlist );
    }

    bool KFRelationAttributeModule::RelationExist( const std::string& listname, uint64 playerid, uint64 targetid )
    {
        return _relation_attribute_logic->RelationExist( listname, playerid, targetid );
    }

    uint32 KFRelationAttributeModule::RelationCount( const std::string& listname, uint64 playerid )
    {
        return _relation_attribute_logic->RelationCount( listname, playerid );
    }

    void KFRelationAttributeModule::AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        return _relation_attribute_logic->AddRelation( listname, relationname, playerid, targetid, bothway );
    }

    void KFRelationAttributeModule::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        return _relation_attribute_logic->RemoveRelation( listname, relationname, playerid, targetid, bothway );
    }

    void KFRelationAttributeModule::AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        return _relation_attribute_logic->AddInvite( listname, relationname, playerid, targetid, message, keeptime );
    }

    bool KFRelationAttributeModule::IsRefuse( const std::string& refusename, uint64 playerid )
    {
        return _relation_attribute_logic->IsRefuse( refusename, playerid );
    }

    void KFRelationAttributeModule::SetRefuse( const std::string& refusename, uint64 playerid, uint32 refusevalue )
    {
        return _relation_attribute_logic->SetRefuse( refusename, playerid, refusevalue );
    }
}