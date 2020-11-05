#ifndef __KF_ENTER_CONFIG_H__
#define __KF_ENTER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFEnterSetting : public KFIntSetting
    {
    public:
        // lua脚本
        std::string _lua_file;

        // lua函数
        std::string _lua_function;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFEnterConfig : public KFConfigT< KFEnterSetting >, public KFInstance< KFEnterConfig >
    {
    public:
        KFEnterConfig()
        {
            _file_name = "enter";
        }

    protected:
        // 创建配置
        KFEnterSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFEnterSetting* kfsetting );
    };
}

#endif