#ifndef __KF_DATA_SETTING_H__
#define __KF_DATA_SETTING_H__

#include "KFrame.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDataSetting
    {
    public:
        KFDataSetting();

        // 属性标记
        bool HaveFlagMask( uint32 mask ) const;

    public:
        // 属性名字
        std::string _name;

        // 属性类型
        uint32 _type;

        // 包含类型
        std::string _contain_class;

        // 初始值
        std::string _init_value;

        // 属性标记
        uint32 _data_mask;

        // 索引字段
        std::string _key_name;

        // 配置索引值
        std::string _config_key_name;

        // 脚本文件
        std::string _lua_file;

        // 脚本函数
        std::string _update_function;
        std::string _add_function;
        std::string _remove_function;
    };
}
#endif
