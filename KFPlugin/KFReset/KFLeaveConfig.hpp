#ifndef __KF_LEAVE_CONFIG_H__
#define __KF_LEAVE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFLeaveSetting : public KFIntSetting
    {
    public:
        // lua脚本
        std::string _lua_file;

        // lua函数
        std::string _lua_function;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFLeaveConfig : public KFConfigT< KFLeaveSetting >, public KFInstance< KFLeaveConfig >
    {
    public:
        KFLeaveConfig()
        {
            _file_name = "leave";
        }

    protected:
        // 创建配置
        KFLeaveSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFLeaveSetting* kfsetting );
    };
}

#endif