#include "KFRelationConfig.hpp"

namespace KFrame
{
    void KFRelationConfig::ReadSetting( KFNode& xmlnode, KFRelationSetting* kfsetting )
    {
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_both_way = xmlnode.GetBoolen( "BothWay" );
        kfsetting->_need_update = xmlnode.GetBoolen( "NeedUpdate" );
        kfsetting->_online_load = xmlnode.GetBoolen( "OnlineLoad" );
        kfsetting->_invite_data_name = xmlnode.GetString( "InviteDataName" );
        kfsetting->_invite_data_count = xmlnode.GetUInt32( "InviteListCount" );
        kfsetting->_invite_list_name = xmlnode.GetString( "InviteListName" );
        kfsetting->_data_list_name = xmlnode.GetString( "DataListName" );
        kfsetting->_invite_keep_time = xmlnode.GetUInt32( "InviteKeepTime" );
        kfsetting->_refuse_name = xmlnode.GetString( "RefuseName" );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRelationSetting* KFRelationConfig::FindRelationSettingByInviteName( const std::string& name )
    {
        for ( auto& iter : _settings._objects )
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