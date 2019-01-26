#ifndef __KF_APP_CONFIG_H__
#define __KF_APP_CONFIG_H__

#include "KFrame.h"
#include "KFSingleton.h"

namespace KFrame
{
    class KFAppSetting
    {
    public:
        KFAppSetting()
        {
            _sort = 0;
        }

        // 名字
        std::string _name;

        // 排序
        uint32 _sort;

        // 配置路径
        std::string _config_file;
    };
    //////////////////////////////////////////////////////////////////////////////

    class KFNode;
    class KFAppConfig : public KFSingleton< KFAppConfig >
    {
    public:
        KFAppConfig() = default;
        ~KFAppConfig() = default;

        // 加载启动配置
        bool LoadStartupConfig( const std::string& file );

    protected:

        // 读取插件配置
        void LoadStarupConfig( const std::string& file, std::string& includefile );
        void ReadPluginSetting( KFNode& root, std::string& includefile );

        // 查找配置
        KFAppSetting* GetStartupSetting( const std::string& name );

        // 添加配置
        void AddStartupSetting( KFAppSetting& setting );

    public:
        // Plugin path
        std::string _plugin_path;

        // 配置
        std::unordered_map< std::string, KFAppSetting > _startups;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _app_config = KFrame::KFAppConfig::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
