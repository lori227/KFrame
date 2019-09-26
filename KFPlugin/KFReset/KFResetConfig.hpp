#ifndef __KF_RESET_CONFIG_H__
#define __KF_RESET_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFTimeConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFResetSetting : public KFIntSetting
    {
    public:
        uint32 _time_id = 0u;
        const KFTimeSetting* _time_setting = nullptr;

        std::string _parent_name;		// 父属性变量名称
        uint32 _key = 0u;				// 属性的变量id
        std::string _data_name;			// 属性变量名称
        uint32 _operate = KFEnum::Set;	// 操作的类型
        uint32 _value = 0u;				// 操作的初始值
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFResetConfig : public KFConfigT< KFResetSetting >, public KFInstance< KFResetConfig >
    {
    public:
        KFResetConfig()
        {
            _file_name = "reset";
        }

        virtual void LoadAllComplete();

    protected:
        // 创建配置
        KFResetSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting );
    };
}

#endif