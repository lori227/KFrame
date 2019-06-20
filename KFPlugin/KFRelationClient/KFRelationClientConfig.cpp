#include "KFRelationClientConfig.hpp"

namespace KFrame
{
    void KFRelationClientConfig::ReadSetting( KFNode& xmlnode, KFRelationSetting* kfsetting )
    {
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_both_way = xmlnode.GetBoolen( "BothWay" );
        kfsetting->_need_update = xmlnode.GetBoolen( "NeedUpdate" );
        kfsetting->_online_load = xmlnode.GetBoolen( "OnlineLoad" );
        kfsetting->_invite_data_name = xmlnode.GetString( "InviteDataName" );
        kfsetting->_invite_data_count = xmlnode.GetUInt32( "InviteListCount" );
        kfsetting->_refuse_name = xmlnode.GetString( "RefuseName" );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRelationSetting* KFRelationClientConfig::FindRelationSettingByInviteName( const std::string& name )
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