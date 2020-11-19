#ifndef __KF_LOADER_MODULE_H__
#define __KF_LOADER_MODULE_H__


/************************************************************************
//    @Module			:    配置加载模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-19
************************************************************************/

#include "KFLoaderConfig.hpp"
#include "KFVersionConfig.hpp"
#include "KFLoaderInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    class KFLoaderModule : public KFLoaderInterface
    {
    public:
        KFLoaderModule();
        ~KFLoaderModule();

        // 加载配置
        virtual void LoadConfig();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

        // 重新加载配置
        virtual void ReloadConfig( const std::string& file );
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        void ProcessReloadCommand( const StringVector& params );

        // 创建config
        virtual void AddConfig( const std::string& name, KFConfig* kfconfig );

        // 查找config
        virtual KFConfig* FindConfig( const std::string& name );

        // 加载配置列表
        void LoadConfigListAndVersion();

        // 加载配置
        void LoadConfigFile( KFConfig* config, const std::string& filename, const std::string& filepath, uint32 loadmask );
    private:
        // 配置列表
        KFLoaderConfig* _kf_loader_config = nullptr;

        // 版本列表
        KFVersionConfig* _kf_version_config = nullptr;

        // 配置列表
        std::unordered_map< std::string, KFConfig* > _config_list;
    };
}



#endif