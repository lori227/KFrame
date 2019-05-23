#include "KFRelationShardConfig.hpp"

namespace KFrame
{
    bool KFRelationShardConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto name = xmlnode.GetString( "Name" );
            auto kfsetting = _relation_list.Create( name );
            kfsetting->_data_name = name;
            kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
            kfsetting->_both_way = xmlnode.GetBoolen( "BothWay" );
            kfsetting->_need_update = xmlnode.GetBoolen( "NeedUpdate" );
            kfsetting->_online_load = xmlnode.GetBoolen( "OnlineLoad" );
            kfsetting->_data_list_name = xmlnode.GetString( "DataListName" );
            kfsetting->_invite_data_name = xmlnode.GetString( "InviteDataName" );
            kfsetting->_invite_list_name = xmlnode.GetString( "InviteListName" );
            kfsetting->_invite_data_count = xmlnode.GetUInt32( "InviteListCount" );
            kfsetting->_invite_keep_time = xmlnode.GetUInt32( "InviteKeepTime" );
            kfsetting->_refuse_name = xmlnode.GetString( "RefuseName" );
            xmlnode.NextNode();
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRelationSetting* KFRelationShardConfig::FindRelationSetting( const std::string& name )
    {
        return _relation_list.Find( name );
    }

}