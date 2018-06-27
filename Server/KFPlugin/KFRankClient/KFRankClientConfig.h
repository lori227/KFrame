#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFRankSetting
	{
	public:
		KFRankSetting()
		{

		}

	public:
		uint32 _match_id;
		uint32 _rank_id;

	};


	
	class KFRankClientConfig : public KFConfig, public KFSingleton< KFRankClientConfig >
	{
	public:
		KFRankClientConfig();
		~KFRankClientConfig();

		// 加载配置
		bool LoadConfig( const char* file );

		// 查找排行榜设定
		const KFRankSetting* FindRankSetting( uint32 matchid ) const;
	public:
		// 匹配配置列表
		KFMap< uint32, uint32, KFRankSetting > _kf_rank_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_rank_config = KFRankClientConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif