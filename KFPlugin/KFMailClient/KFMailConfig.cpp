#include "KFMailConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMailSetting* KFMailConfig::FindMailSetting( uint32 configid ) const
    {
        return _mail_setting.Find( configid );
    }

    bool KFMailConfig::LoadConfig( const std::string& file )
    {
        _mail_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto setting = config.FindNode( "item" );
        while ( setting.IsValid() )
        {
            auto configid = setting.GetUInt32( "ConfigId" );
            auto kfsetting = _mail_setting.Create( configid );

            kfsetting->_config_id = configid;
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
            kfsetting->_rewards.Parse( stragent, __FUNC_LINE__ );

            setting.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}