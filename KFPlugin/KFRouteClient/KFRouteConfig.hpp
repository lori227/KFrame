#ifndef __KF_ROUTE_CONFIG_H__
#define __KF_ROUTE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFRouteSetting : public KFStrSetting
    {
    public:
        // 模块名字
        std::string _module;

        // 进程名字
        std::string _process;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFRouteConfig : public KFConfigT< KFRouteSetting >, public KFInstance< KFRouteConfig >
    {
    public:
        KFRouteConfig()
        {
            _file_name = "route";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRouteSetting* kfsetting );
    };
}

#endif