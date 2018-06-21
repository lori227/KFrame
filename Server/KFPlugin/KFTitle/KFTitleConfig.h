#ifndef __KF_TITLE_CONFIG_H__
#define __KF_TITLE_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	namespace KFTitleEnum
	{
		// 活动函数枚举
		enum EConstDefine
		{

		};
	}
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFTitleSetting
	{
	public:

	};


	class KFTitleConfig : public KFConfig, public ISingleton< KFTitleConfig >
	{
	public:
		KFTitleConfig();
		~KFTitleConfig();

		bool LoadConfig( const char* file );

	protected:

	public:
		// 称号列表
		std::map< uint32, KFTitleSetting > _title_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto* _kf_title_config = KFTitleConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif