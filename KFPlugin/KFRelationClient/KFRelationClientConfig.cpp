#include "KFRelationClientConfig.hpp"

namespace KFrame
{
    bool KFRelationClientConfig::LoadConfig( const std::string& file )
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "item" );
        while ( xmlnode.IsValid() )
        {
            auto name = xmlnode.GetString( "Name" );
            auto kfsetting = _relation_list.Create( name );
            kfsetting->_data_name = name;
            kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
            kfsetting->_both_way = xmlnode.GetBoolen( "BothWay" );
            kfsetting->_need_update = xmlnode.GetBoolen( "NeedUpdate" );
            kfsetting->_online_load = xmlnode.GetBoolen( "OnlineLoad" );
            kfsetting->_invite_data_name = xmlnode.GetString( "InviteDataName" );
            kfsetting->_invite_data_count = xmlnode.GetUInt32( "InviteListCount" );
            kfsetting->_refuse_name = xmlnode.GetString( "RefuseName" );

            xmlnode.NextNode();
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRelationSetting* KFRelationClientConfig::FindRelationSetting( const std::string& name )
    {
        return _relation_list.Find( name );
    }

    const KFRelationSetting* KFRelationClientConfig::FindRelationSettingByInviteName( const std::string& name )
    {
        for ( auto& iter : _relation_list._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_invite_data_name == name )
            {
                return kfsetting;
            }
        }

        return nullptr;
    }
}