#ifndef __KF_CLASS_SETTING_H__
#define __KF_CLASS_SETTING_H__

#include "KFDataSetting.h"

namespace KFrame
{
	// 对象类定义
	////////////////////////////////////////////////////////////////////////////////////////
	__ST_CLASS__( KFClassSetting )
	{
	public:
		KFClassSetting();
		~KFClassSetting();

		void AddDataSetting( KFDataSetting& setting );

		const KFDataSetting* GetDataSetting( const std::string& name ) const;
		const KFDataSetting* GetDataSetting( uint32 index ) const;

		bool IsChildData( const std::string& name ) const;

	public:
		// 名称
		std::string _class;

		// 属性列表
		std::map< std::string, KFDataSetting > _static_data;
		std::map< uint32, std::string > _indexs;
	};
}
#endif
