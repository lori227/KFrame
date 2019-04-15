#ifndef __KF_DEPLOY_CONFIG_H__
#define __KF_DEPLOY_CONFIG_H__

#include "KFrame.h"
#include "KFSingleton.h"

namespace KFrame
{
    class KFDeploySetting
    {
    public:
        uint32 _sort = 0;

        // 名字
        std::string _name;

        // 配置路径
        std::string _config_file;
    };
    //////////////////////////////////////////////////////////////////////////////

    class KFDeployConfig : public KFSingleton< KFDeployConfig >
    {
    public:
        // 加载启动配置
        bool LoadStartupConfig( const std::string& file );

    protected:
        void ReadStartupConfig( const std::string& file );

        // 添加配置
        void AddStartupSetting( KFDeploySetting& setting );

    public:
        // Plugin path
        std::string _plugin_path;

        // 配置
        std::list< KFDeploySetting > _startups;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _deploy_config = KFrame::KFDeployConfig::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
