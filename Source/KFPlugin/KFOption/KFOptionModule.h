#ifndef __KF_OPTION_MOUDLE_H__
#define __KF_OPTION_MOUDLE_H__

/************************************************************************
//    @Moudle			:    全局配置模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-24
************************************************************************/

#include "KFrame.h"
#include "KFOptionInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFOptionModule : public KFOptionInterface
	{
	public:
		KFOptionModule();
		~KFOptionModule();
		
		// 初始化
		virtual void InitModule();

		// 逻辑
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// 获得string配置
		virtual const std::string& GetString( const std::string& name, uint32 key = 0 );

		// 获得uint32配置
		virtual uint32 GetUInt32( const std::string& name, uint32 key = 0 );
	};
}



#endif