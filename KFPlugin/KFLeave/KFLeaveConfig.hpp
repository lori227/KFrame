#ifndef __KF_LEAVE_CONFIG_H__
#define __KF_LEAVE_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

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
    class KFLeaveConfig : public KFIntConfigT< KFLeaveSetting >, public KFSingleton< KFLeaveConfig >
    {
    public:
        KFLeaveConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFLeaveSetting >( file, isclear )
        {
        }

    protected:
        // 创建配置
        KFLeaveSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFLeaveSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_leave_config = KFLeaveConfig::Instance( "leave.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif