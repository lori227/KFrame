#ifndef __KF_RESET_CONFIG_H__
#define __KF_RESET_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class KFResetData
	{
	public:
		KFResetData()
		{
			_key = 0;
			_value = 0;
		}

	public:
		std::string _parent_name;	// 父属性变量名称
		uint64 _key;				// 属性的变量id
		std::string _data_name;		// 属性变量名称
		uint64 _value;				// 重置的初始值
	};
	
	__ST_CLASS__( KFResetSetting )
	{
	public:
		uint64 _note_id;			// 保存时间的变量id
		uint32 _reset_time_type;	// 重置时间类型
		uint32 _reset_time_value;	// 重置的时间点( 根据类型来判断 )
		std::vector< KFResetData > _reset_data_list;
	};

	////////////////////////////////////////////////////////////////////////////////////

	class KFResetConfig : public KFConfig, public KFSingleton< KFResetConfig >
	{
	public:
		KFResetConfig();
		~KFResetConfig();

		bool LoadConfig( const char* file );

	protected:
		// 添加重置数据
		void AddResetData( uint64 noteid, uint32 resettype, uint32 resettime, const KFResetData& resetdata );

	public:
		// 称号列表
		KFMap< uint32, uint32, KFResetSetting > _reset_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_reset_config = KFResetConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif