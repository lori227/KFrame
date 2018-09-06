#include "KFMailConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFMailConfig::KFMailConfig()
    {

    }

    KFMailConfig::~KFMailConfig()
    {

    }

    const KFMailSetting* KFMailConfig::FindMailSetting( uint32 configid ) const
    {
        return _mail_setting.Find( configid );
    }

    bool KFMailConfig::LoadConfig()
    {
        _mail_setting.Clear();

        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto setting = config.FindNode( "Setting" );
            while ( setting.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFMailSetting );

                kfsetting->_config_id = setting.GetUInt32( "ConfigId" );
                kfsetting->_type = setting.GetUInt32( "Type" );
                kfsetting->_reply_id = setting.GetUInt32( "ReplyId" );

                kfsetting->_title = setting.GetString( "Title" );
                KFUtility::ReplaceString( kfsetting->_title, " ", "" );

                kfsetting->_content = setting.GetString( "Content" );
                KFUtility::ReplaceString( kfsetting->_content, " ", "" );

                kfsetting->_valid_time = setting.GetUInt32( "ValidTime" ) * KFTimeEnum::OneDaySecond;
                kfsetting->_extend = setting.GetString( "Extend" );
                KFUtility::ReplaceString( kfsetting->_extend, " ", "" );

                auto stragent = setting.GetString( "Reward" );
                kfsetting->_rewards.ParseFromString( stragent, __FUNC_LINE__ );
                _mail_setting.Insert( kfsetting->_config_id, kfsetting );

                setting.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}