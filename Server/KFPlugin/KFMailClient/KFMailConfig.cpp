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

	const KFMailSetting* KFMailConfig::FindMailSetting( uint32 configid )
	{
		return _mail_setting.Find( configid );
	}

	bool KFMailConfig::LoadConfig( const char* file )
	{
		_mail_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto setting = config.FindNode( "Setting" );
			while ( setting.IsValid() )
			{
				auto kfsetting = __KF_CREATE__( KFMailSetting );

				kfsetting->_config_id = setting.GetUInt32( "ConfigId" );
				kfsetting->_reply_id = setting.GetUInt32( "ReplyId" );
				kfsetting->_type = setting.GetUInt32( "Type" );

				kfsetting->_title = setting.GetString( "Title" );
				KFUtility::ReplaceString( kfsetting->_title, " ", "" );
				
				kfsetting->_content = setting.GetString( "Content" );
				KFUtility::ReplaceString( kfsetting->_content, " ", "" );

				kfsetting->_valid_time = setting.GetUInt32( "ValidTime" ) * TimeEnum::OneDaySecond;

				kfsetting->_daily_get_limit = setting.GetUInt32( "DailyGetLimit" );
				kfsetting->_daily_send_limit = setting.GetUInt32( "DailySendLimit" );

				kfsetting->_extend = setting.GetString( "Extend" );
				auto stragent = setting.GetString( "AgentList" );
				kfsetting->_rewards.ParseFromString( stragent, __FUNCTION_LINE__ );
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