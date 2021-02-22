#ifndef __KF_LOADER_MODULE_H__
#define __KF_LOADER_MODULE_H__


/************************************************************************
//    @Module			:    配置加载模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-19
************************************************************************/

#include "KFLoaderInterface.h"
#include "KFVersionConfig.hpp"
#include "KFConfig/KFLoaderConfig.hpp"

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
        virtual void ReloadConfig();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        void ProcessReloadCommand( const StringVector& params );

        // 创建config
        virtual void AddConfig( const std::string& name, KFConfig* config );

        // 查找config
        virtual KFConfig* FindConfig( const std::string& name );

        // 加载配置列表
        void LoadConfigListAndVersion();

        // 加载配置
        void LoadConfigFile( KFConfig* config, const std::string& filepath, uint32 cleartype );

        // 判断是否需要重新加载
        const std::string& CheckConfigFileNeedReload( KFConfig* config, const KFConfigData* config_data, StringSet& reload_list );
    private:
        // 配置列表
        KFLoaderConfig* _kf_loader_config = nullptr;

        // 版本列表
        KFVersionConfig* _kf_version_config = nullptr;

        // 配置列表
        std::unordered_map<std::string, KFConfig*> _config_list;
    };
}



#endif