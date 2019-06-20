#ifndef __KF_ENTER_CONFIG_H__
#define __KF_ENTER_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

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
    class KFEnterConfig : public KFIntConfigT< KFEnterSetting >, public KFSingleton< KFEnterConfig >
    {
    public:
        KFEnterConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFEnterSetting >( file, isclear )
        {
        }

    protected:
        // 创建配置
        KFEnterSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFEnterSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_enter_config = KFEnterConfig::Instance( "enter.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif