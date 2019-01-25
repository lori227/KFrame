#ifndef __KF_STARTUP_H__
#define __KF_STARTUP_H__

#include "KFMap.h"
#include "KFLibrary.h"
#include "KFSingleton.h"

namespace KFrame
{
    class KFLibrary;
    class KFAppSetting;
    class KFStartup : public KFSingleton< KFStartup >
    {
    public:
        KFStartup() = default;
        ~KFStartup() = default;

        // 初始化
        bool InitStartup( const std::string& file );

        // 加载插件
        bool LoadPlugin();

        // 关闭
        void ShutDown();

    public:
        bool LoadPluginLibrary( const std::string& file, const KFAppSetting* appsetting  );

        // 卸载
        bool UnLoadPluginLibrary( const std::string& pluginname );

    private:
        // 插件列表
        KFMap< std::string, const std::string&, KFLibrary > _kf_library;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _kf_startup = KFrame::KFStartup::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
