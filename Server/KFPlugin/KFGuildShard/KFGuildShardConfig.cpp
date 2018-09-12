#include "KFGuildShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFGuildShardConfig::KFGuildShardConfig()
    {
        _max_guildlist_page = _invalid_int;
    }

    KFGuildShardConfig::~KFGuildShardConfig()
    {

    }


    bool KFGuildShardConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        {
            auto xmlnode = config.FindNode( "Guilds" );
            if ( xmlnode.IsValid() )
            {
                auto childnode = xmlnode.FindNode( "Guild" );
                while ( childnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFGuildSetting );
                    kfsetting->_level = childnode.GetUInt32( "Level" );
                    kfsetting->_max_member = childnode.GetUInt32( "MaxMember" );
                    kfsetting->_upgrade_activeness = childnode.GetUInt32( "UpgradeActiveness" );
                    kfsetting->_degrade_activeness = childnode.GetUInt32( "DegradeActiveness" );
                    kfsetting->_max_log = childnode.GetUInt32( "MaxLog" );
                    kfsetting->_max_applylist = childnode.GetUInt32( "MaxApplylist" );
                    AddGuildSetting( kfsetting );
                    childnode.NextNode();
                }

            }

        }
        {
            auto shownode = config.FindNode( "GuildRanks" );
            if ( shownode.IsValid() )
            {
                auto childnode = shownode.FindNode( "ShowData" );
                while ( childnode.IsValid() )
                {
                    auto dataname = childnode.GetString( "Name" );
                    _show_data.push_back( dataname );
                    childnode.NextNode();
                }

            }
        }

        {
            auto xmlnode = config.FindNode( "GuildMmebers" );
            if ( xmlnode.IsValid() )
            {
                auto childnode = xmlnode.FindNode( "GuildMember" );
                while ( childnode.IsValid() )
                {
                    auto title = childnode.GetUInt32( "Title" );
                    uint32 datamask = 0;
                    if ( childnode.GetString( "Invite" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Invite_Data );
                    }

                    if ( childnode.GetString( "Review" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Review_Data );
                    }

                    if ( childnode.GetString( "Dissolve" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Dissolve_Data );
                    }

                    if ( childnode.GetString( "Dismissal" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Dismissal_Data );
                    }

                    if ( childnode.GetString( "Appoint" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Appoint_Data );
                    }

                    if ( childnode.GetString( "Modify" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildShardEnum::Mask_Modify_Data );
                    }
                    childnode.NextNode();
                }
            }
        }

        {
            auto optionnode = config.FindNode( "option" );
            if ( optionnode.IsValid() )
            {
                _max_guildlist_page = optionnode.GetUInt32( "MaxGuildListPage" );
            }


        }
        //////////////////////////////////////////////////////////////////

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

    bool KFGuildShardConfig::IsOwnPower( uint32 title, uint32 datamask ) const
    {
        auto iter = _title_power.find( title );
        if ( iter == _title_power.end() )
        {
            return false;
        }
        return KFUtility::HaveBitMask( iter->second, datamask );
    }
}