#include "KFGuildShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFGuildShardConfig::KFGuildShardConfig()
    {

    }

    KFGuildShardConfig::~KFGuildShardConfig()
    {

    }


    bool KFGuildShardConfig::LoadConfig()
    {
        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            {
                auto xmlnode = config.FindNode( "Guild" );
                while ( xmlnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFGuildSetting );
                    kfsetting->_level = xmlnode.GetUInt32( "Level" );
                    kfsetting->_max_member = xmlnode.GetUInt32( "MaxMember" );
                    kfsetting->_upgrade_activeness = xmlnode.GetUInt32( "UpgradeActiveness" );
                    kfsetting->_degrade_activeness = xmlnode.GetUInt32( "DegradeActiveness" );
                    kfsetting->_max_log = xmlnode.GetUInt32( "MaxLog" );
                    kfsetting->_max_applylist = xmlnode.GetUInt32( "MaxApplylist" );
                    AddGuildSetting( kfsetting );
                    xmlnode.NextNode();
                }

            }
            {
                auto shownode = config.FindNode( "ShowData" );
                while ( shownode.IsValid() )
                {
                    auto dataname = shownode.GetString( "Name" );
                    _show_data.push_back( dataname );
                    shownode.NextNode();
                }
            }

            {
                auto optionnode = config.FindNode( "option" );
                _max_guildlist_page = optionnode.GetUInt32( "MaxGuildListPage" );
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    void KFGuildShardConfig::AddGuildSetting( KFGuildSetting* kfsetting )
    {
        _kf_guild_setting.Insert( kfsetting->_level, kfsetting );
    }

    const KFGuildSetting* KFGuildShardConfig::FindGuildSetting( uint32 level ) const
    {
        return _kf_guild_setting.Find( level );
    }

}