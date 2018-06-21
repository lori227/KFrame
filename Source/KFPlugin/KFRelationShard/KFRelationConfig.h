#ifndef __KF_FRIEND_CONFIG_H__
#define __KF_FRIEND_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	namespace KFFriendEnum
	{
		enum EConstDefine
		{
			Team = 1,	//组队
			Win = 2,	//胜利
			Laud = 3,	//敬酒
		};
	}
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFRelationConfig : public KFConfig, public KFSingleton< KFRelationConfig >
	{
	public:
		KFRelationConfig();
		~KFRelationConfig();

		// 加载配置
		bool LoadConfig( const char* file );

		// 返回好友度上限
		uint32 getMaxFriendLine( uint32 operate );

	public:
		// 最大好友数量
		uint32 _max_friend_count;

		// 最大申请数量
		uint32 _max_invite_count;

		// 数量保存时间
		uint32 _invite_keep_time;
		
		// 每次组队添加好友度
		uint32 _single_team_friend_liness;

		// 每日组队最大好友度
		uint32 _max_team_friend_liness;

		// 每次胜利添加好友度
		uint32 _single_win_friend_liness;

		// 每日胜利最大好友度
		uint32 _max_win_friend_liness;

		// 敬酒添加好友度
		uint32 _jing_jiu_friend_liness;

		// 每日最大敬酒添加好友度
		uint32 _max_jing_jiu_friend_liness;

		// 好友度重置时间
		uint32 _friend_liness_reset_time;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_relation_config = KFRelationConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif