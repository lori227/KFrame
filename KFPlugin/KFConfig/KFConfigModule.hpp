#ifndef __KF_CONFIG_MODULE_H__
#define __KF_CONFIG_MODULE_H__


/************************************************************************
//    @Module			:    配置管理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-19
************************************************************************/

#include "KFConfigConfig.hpp"
#include "KFConfigInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    class KFConfigModule : public KFConfigInterface
    {
    public:
        KFConfigModule();
        ~KFConfigModule();

        // 加载配置
        virtual void LoadConfig();

        // 关闭
        virtual void ShutDown();

        // 重新加载配置
        virtual void ReloadConfig( const std::string& file );
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 创建config
        virtual void AddConfig( const std::string& name, KFConfig* kfconfig );

        // 查找config
        virtual KFConfig* FindConfig( const std::string& name );

        // 加载配置
        void LoadConfigFile( KFConfig* config, const std::string& file, uint32 loadmask );

    private:
        // 配置列表
        KFConfigConfig* _kf_config_config = nullptr;

        // 配置列表
        std::unordered_map< std::string, KFConfig* > _config_list;
    };
}



#endif