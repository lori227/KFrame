#include "KFGuildClientConfig.h"

namespace KFrame
{

    bool KFGuildSetting::IsValidMedal( uint32 medal ) const
    {
        auto strmedal = KFUtility::ToString( medal );
        return _medal_list.find( strmedal ) != std::string::npos;
    }

    bool KFGuildActivenessSetting::CheckCanUpdate( uint32 key, uint32 operate ) const
    {
        if ( _key != _invalid_int && _key != key )
        {
            return false;
        }

        if ( operate != _operate )
        {
            return false;
        }

        return true;
    }

    uint32 KFGuildActivenessSetting::GetAddActivenessValue( uint32 value, uint32 ownactiveness, uint32& usevalue )
    {
        auto addactiveness = _invalid_int;
        //单日限制
        if ( _day_limit <= ownactiveness )
        {
            return addactiveness;
        }
        auto restactiveness = _day_limit - ownactiveness;
        switch ( _trigger_type )
        {

        case KFGuildEnum::Finish:
        {
            if ( value >= _done_value )
            {
                usevalue += __MIN__( _done_value, value );
                addactiveness = __MIN__( restactiveness, _add_activeness_value );
            }

        }
        break;
        case KFGuildEnum::Amass:
        {
            auto totalusevalue = usevalue + value;
            // 计算可以触发增加活跃度的次数
            auto resttime = totalusevalue / _done_value - usevalue / _done_value;
            auto incrbyactiveness = resttime * _add_activeness_value;
            addactiveness = __MIN__( incrbyactiveness, restactiveness );
            usevalue = totalusevalue;
        }
        default:
            break;
        }
        return addactiveness;
    }

    KFGuildConfig::KFGuildConfig()
    {
        _title_power.clear();
        _max_level = _invalid_int;
        _keep_time = _invalid_int;
        _max_guild_log_page = _invalid_int;
        _max_week_activeness = _invalid_int;
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
                    auto level = childnode.GetUInt32( "Level" );
                    _max_level = __MAX__( level, _max_level );

                    auto kfsetting = _guild_setting.Create( level );
                    kfsetting->_level = level;
                    kfsetting->_max_member = childnode.GetUInt32( "MaxMember" );
                    kfsetting->_upgrade_activeness = childnode.GetUInt32( "UpgradeActiveness" );
                    kfsetting->_degrade_activeness = childnode.GetUInt32( "DegradeActiveness" );
                    auto costvalue = childnode.GetString( "Cost" );
                    kfsetting->_cost_item.Parse( costvalue, __FUNC_LINE__ );
                    kfsetting->_medal_list = childnode.GetString( "MedalList" );
                    kfsetting->_max_log = childnode.GetUInt32( "MaxLog" );


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
            auto xmlnode = config.FindNode( "GuildActinesses" );
            if ( xmlnode.IsValid() )
            {
                auto childnode = xmlnode.FindNode( "GuildActiness" );
                while ( childnode.IsValid() )
                {
                    auto kfsetting = __KF_NEW__( KFGuildActivenessSetting );
                    kfsetting->_id = childnode.GetUInt32( "Id" );
                    kfsetting->_add_activeness_value = childnode.GetUInt32( "AddActivenessOnce" );
                    kfsetting->_parent_name = childnode.GetString( "ParentName" );
                    kfsetting->_data_name = childnode.GetString( "DataName" );
                    kfsetting->_key = childnode.GetUInt32( "Key" );
                    kfsetting->_operate = childnode.GetUInt32( "Operate" );
                    kfsetting->_day_limit = childnode.GetUInt32( "DayLimit" );
                    kfsetting->_trigger_type = childnode.GetUInt32( "TriggerType" );
                    kfsetting->_done_value = childnode.GetUInt32( "DoneValue" );
                    AddKFActivenessSetting( kfsetting );
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
                _max_week_activeness = optionnode.GetUInt32( "MaxWeekActiveness" );
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

    void KFGuildConfig::AddKFActivenessSetting( KFGuildActivenessSetting* kfsetting )
    {
        AddKFTypeActiveness( kfsetting );
        _guild_activeness_setting.Insert( kfsetting->_id, kfsetting );
    }

    void KFGuildConfig::AddKFTypeActiveness( KFGuildActivenessSetting* kfsetting )
    {
        auto iter = _object_types.find( kfsetting->_parent_name );
        if ( _object_types.end() == iter )
        {
            iter = _object_types.insert( std::make_pair( kfsetting->_parent_name, KFGuildActivenessTypes() ) ).first;
        }
        iter->second.AddKFGuildActivenessType( kfsetting );
    }

    const KFGuildActivenessType* KFGuildConfig::FindTypeActiveness( const std::string& parentname, const std::string& dataname ) const
    {
        auto iter = _object_types.find( parentname );
        if ( _object_types.end() == iter )
        {
            return nullptr;
        }
        return iter->second.FindKFGuildActivenessType( dataname );
    }


    void KFGuildActivenessTypes::AddKFGuildActivenessType( KFGuildActivenessSetting* setting )
    {
        auto iter = _guild_activenesstype.find( setting->_data_name );
        if ( _guild_activenesstype.end() == iter )
        {
            iter = _guild_activenesstype.insert( std::make_pair( setting->_data_name, KFGuildActivenessType() ) ).first;
        }
        iter->second.AddKFGuildActivenessType( setting );
    }

    const KFGuildActivenessType* KFGuildActivenessTypes::FindKFGuildActivenessType( const std::string& dataname ) const
    {
        auto iter = _guild_activenesstype.find( dataname );
        if ( iter == _guild_activenesstype.end() )
        {
            return nullptr;
        }
        return &iter->second;
    }
}