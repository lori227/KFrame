#ifndef __KF_CONFIG_INTERFACE_H__
#define __KF_CONFIG_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFConfig
    {
    public:
        // 加载配置
        virtual bool LoadConfig( const std::string& file )
        {
            _file = file;
            return LoadConfig();
        }

        virtual bool LoadConfig() = 0;

    protected:
        // 配置文件
        std::string _file;
    };

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    class KFConfigInterface : public KFModule
    {
    public:
        // 加载一个配置
        virtual void AddConfig( KFConfig* config,  const std::string& file, bool canreload ) = 0;

        // 删除配置
        virtual void RemoveConfig( KFConfig* config ) = 0;
    };


    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_config, KFConfigInterface );
    ///////////////////////////////////////////////////////////////////////
#define __KF_ADD_CONFIG__( kfconfig, reload ) \
    _kf_config->AddConfig( kfconfig, _kf_plugin->_config, reload )

#define __KF_REMOVE_CONFIG__( kfconfig ) \
    _kf_config->RemoveConfig( kfconfig )
    ///////////////////////////////////////////////////////////////////////

}



#endif