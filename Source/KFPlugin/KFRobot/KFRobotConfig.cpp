#include "KFRobotConfig.h"

namespace KFrame
{
	KFRobotConfig::KFRobotConfig()
	{
		_auth_server_ip = "http://192.168.1.9:7777/";
		_robot_name = "robot";
		_robot_count = 0;
		_start_count = 0;
		_login_span_time = 3000;
		_verify_channel = 1;
		_server_channel = 4;
		_account_id_base = 90000;
		_friend_switch = 1;
		_cd_key = "";
		_oddStateList = "";
		_evenStateList = "";
		_state_rep_time = 0;
		_next_state_cryl_time = 0;
	}

	KFRobotConfig::~KFRobotConfig()
	{

	}

	bool KFRobotConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			/////////////////////////////////////////////////////
			{
				auto xmlnode = config.FindNode( "Connection" );
				_auth_server_ip = xmlnode.GetString( "AuthServerIp" );
				_verify_channel = xmlnode.GetUInt32( "VerifyChannel" );
				_server_channel = xmlnode.GetUInt32( "ServerChanel" );
			}

			{
				auto xmlnode = config.FindNode( "Robot" );
				_robot_name = xmlnode.GetString( "RobotName" );
				_account_id_base = xmlnode.GetUInt32( "AccountidBase" );
				_robot_count = xmlnode.GetUInt32( "RobotCount" );
				_login_span_time = xmlnode.GetUInt32( "LoginSpanTime" );
			}

			{
				auto xmlnode = config.FindNode( "Friend" );
				_friend_switch = xmlnode.GetUInt32( "FriendSwitch" );
			}

			{
				auto xmlnode = config.FindNode( "Gift" );
				_cd_key = xmlnode.GetString( "CDKEY" );
			}

			{
				auto xmlnode = config.FindNode( "State" );
				_oddStateList = xmlnode.GetString( "OddStateList" );
				_evenStateList = xmlnode.GetString( "EvenStateList" );
				_state_rep_time = xmlnode.GetUInt32( "StateRepTime" );
				_next_state_cryl_time = xmlnode.GetUInt32( "NextStateCylTime" );
			}

		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}