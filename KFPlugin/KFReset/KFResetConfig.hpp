#ifndef __KF_RESET_CONFIG_H__
#define __KF_RESET_CONFIG_H__

#include "KFZConfig/KFConfig.h"

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
            _operate = KFEnum::Set;
        }

    public:
        std::string _parent_name;	// 父属性变量名称
        uint32 _key;				// 属性的变量id
        std::string _data_name;		// 属性变量名称
        uint32 _operate;			// 操作的类型
        uint32 _value;				// 操作的初始值
    };

    class KFResetSetting : public KFIntSetting
    {
    public:
        uint32 _time_type;		// 重置时间类型
        uint32 _time_value;		// 重置的时间点( 根据类型来判断 )
        uint32 _time_hour;		// 时间点( 小时 )
        std::vector< KFResetData > _reset_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFResetConfig : public KFIntConfigT< KFResetSetting >, public KFInstance< KFResetConfig >
    {
    public:
        KFResetConfig()
        {
            _file_name = "reset";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting );
    };
}

#endif