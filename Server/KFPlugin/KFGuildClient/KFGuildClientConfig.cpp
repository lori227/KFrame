#include "KFGuildClientConfig.h"

namespace KFrame
{

    bool KFGuildSetting::IsValidMedal( uint32 medal ) const
    {
        auto strmedal = KFUtility::ToString( medal );
        return _medal_list.find( strmedal ) != std::string::npos;
    }


    KFGuildConfig::KFGuildConfig()
    {

    }

    KFGuildConfig::~KFGuildConfig()
    {

    }

    bool KFGuildConfig::LoadConfig()
    {
        _guild_setting.Clear();

        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFGuildSetting );
                kfsetting->_level = xmlnode.GetUInt32( "Level" );

                auto costvalue = xmlnode.GetString( "Cost" );
                kfsetting->_cost_item.ParseFromString( costvalue, __FUNC_LINE__ );
                kfsetting->_max_member = xmlnode.GetUInt32( "MaxMember" );
                kfsetting->_upgrade_activeness = xmlnode.GetUInt32( "UpgradeActiveness" );
                kfsetting->_degrade_activeness = xmlnode.GetUInt32( "DegradeActiveness" );
                kfsetting->_max_log = xmlnode.GetUInt32( "MaxLog" );
                kfsetting->_medal_list = xmlnode.GetString( "MedalList" );

                _guild_setting.Insert( kfsetting->_level, kfsetting );
                xmlnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }
        return true;
    }

    const KFGuildSetting* KFGuildConfig::FindGuildSetting( uint32 level ) const
    {
        return _guild_setting.Find( level );
    }

}