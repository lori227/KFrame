#ifndef __KF_STARTUP_H__
#define __KF_STARTUP_H__

#include "KFMap.h"
#include "KFSingleton.h"
#include "KFDeployLibrary.h"

namespace KFrame
{
    class KFDeploySetting;
    class KFDeployStartup : public KFSingleton< KFDeployStartup >
    {
    public:
        // 初始化
        bool InitStartup( const std::string& file );

        // 加载插件
        bool LoadPlugin();

        // 关闭
        void ShutDown();

    public:
        bool LoadPluginLibrary( const std::string& file, const KFDeploySetting* appsetting  );

        // 卸载
        bool UnLoadPluginLibrary( const std::string& pluginname );

    private:
        // 插件列表
        KFHashMap< std::string, const std::string&, KFDeployLibrary > _kf_library;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _kf_deploy_startup = KFrame::KFDeployStartup::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
