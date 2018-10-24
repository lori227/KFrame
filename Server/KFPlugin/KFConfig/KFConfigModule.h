#ifndef __KF_CONFIG_MODULE_H__
#define __KF_CONFIG_MODULE_H__


/************************************************************************
//    @Module			:    配置管理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-19
************************************************************************/

#include "KFrame.h"
#include "KFConfigInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"

namespace KFrame
{
    class KFConfigData
    {
    public:
        KFConfigData()
        {
            _config = nullptr;
            _can_reload = false;
        }

    public:
        KFConfig* _config;
        std::string _module;
        std::string _file;
        bool _can_reload;
    };
    ////////////////////////////////////////////////////////////////
    class KFConfigModule : public KFConfigInterface
    {
    public:
        KFConfigModule() = default;
        ~KFConfigModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

        // 加载配置
        virtual void LoadConfig();

        // 加载一个配置
        virtual void AddConfig( KFConfig* config, const std::string& module, const std::string& file, bool canreload );

        // 删除配置
        virtual void RemoveConfig( const std::string& module );

        // 加载配置
        void LoadConfig( KFConfig* config, const std::string& module, const std::string& file );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 重新加载配置
        __KF_COMMAND_FUNCTION__( LoadConfig );

    private:
        // 配置列表
        KFMap< std::string, const std::string&, KFConfigData > _kf_config_data;
    };
}



#endif