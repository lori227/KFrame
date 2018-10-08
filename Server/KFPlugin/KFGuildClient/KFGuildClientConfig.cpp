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
        _title_power.clear();
        _max_level = _invalid_int;
        _keep_time = _invalid_int;
        _max_guild_log_page = _invalid_int;
    }

    KFGuildConfig::~KFGuildConfig()
    {

    }

    bool KFGuildConfig::LoadConfig()
    {
        _guild_setting.Clear();

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
                    auto costvalue = childnode.GetString( "Cost" );
                    kfsetting->_cost_item.ParseFromString( costvalue, __FUNC_LINE__ );
                    kfsetting->_medal_list = childnode.GetString( "MedalList" );
                    kfsetting->_max_log = childnode.GetUInt32( "MaxLog" );

                    _max_level = __MAX__( kfsetting->_level, _max_level );
                    _guild_setting.Insert( kfsetting->_level, kfsetting );
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
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Invite_Data );
                    }

                    if ( childnode.GetString( "Review" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Review_Data );
                    }

                    if ( childnode.GetString( "Dissolve" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Dissolve_Data );
                    }

                    if ( childnode.GetString( "Dismissal" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Dismissal_Data );
                    }

                    if ( childnode.GetString( "Appoint" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Appoint_Data );
                    }

                    if ( childnode.GetString( "Modify" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Modify_Data );
                    }

                    if ( childnode.GetString( "Upgrade" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Upgrade_Data );
                    }

                    if ( childnode.GetString( "Manger" ) == "1" )
                    {
                        KFUtility::AddBitMask< uint32 >( datamask, KFGuildEnum::Mask_Manger_Data );
                    }

                    _title_power.insert( std::make_pair( title, datamask ) );
                    childnode.NextNode();
                }
            }
        }

        {
            auto optionnode = config.FindNode( "option" );
            if ( optionnode.IsValid() )
            {
                _keep_time = optionnode.GetUInt32( "Keeptime" );
                _review_switch = optionnode.GetString( "ReviewSwitch" );
                _req_level = optionnode.GetString( "ReqLevel" );
                _max_guild_log_page = optionnode.GetUInt32( "MaxGuildLogPage" );
            }
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFGuildSetting* KFGuildConfig::FindGuildSetting( uint32 level ) const
    {
        return _guild_setting.Find( level );
    }

    bool KFGuildConfig::IsOwnPower( uint32 title, uint32 datamask ) const
    {
        auto iter = _title_power.find( title );
        if ( iter == _title_power.end() )
        {
            return false;
        }
        return KFUtility::HaveBitMask( iter->second, datamask );
    }

    bool KFGuildConfig::IsValidTitle( uint32 title ) const
    {
        return _title_power.find( title ) != _title_power.end();
    }

    bool KFGuildConfig::IsVaildSwitch( const std::string& type, uint32 flag )
    {
        if ( type == __KF_STRING__( reqlevel ) )
        {
            return  std::string::npos != _req_level.find( __TO_STRING__( flag ) );
        }
        else if ( type == __KF_STRING__( reviewswitch ) )
        {
            return std::string::npos != _review_switch.find( __TO_STRING__( flag ) );
        }
        return false;
    }


}