#ifndef __KF_PLAYER_CONFIG_H__
#define __KF_PLAYER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	class KFPlayerConfig : public KFConfig, public KFSingleton< KFPlayerConfig >
	{
	public:
		KFPlayerConfig();
		~KFPlayerConfig();

		// 加载配置
		bool LoadConfig( const char* file );

	public:
		// 每日变量刷新时间( 小时 )
		uint32 _daily_refresh_hour;

		// 每周几变量刷新
		uint32 _day_of_week;

		// 每月几号变量刷新
		uint32 _day_of_month;
	};

	///////////////////////////////////////////////////////////////////////////
	static auto _kf_player_config = KFPlayerConfig::Instance();
	///////////////////////////////////////////////////////////////////////////
}
#endif