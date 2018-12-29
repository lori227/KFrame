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

        bool LoadServerConfig( const std::string& file );
        bool LoadCommonConfig( const std::string& file );

        // 读取插件配置
        void ReadPluginSetting( KFNode& root );

        // 查找配置
        KFAppSetting* GetStartupSetting( const std::string& name );

        // 添加配置
        void AddStartupSetting( KFAppSetting& setting );

    protected:
        // 公共的插件文件
        std::string _common_startup_file;

    public:
        // appname
        std::string _app_name;

        // apptype
        std::string _app_type;

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
