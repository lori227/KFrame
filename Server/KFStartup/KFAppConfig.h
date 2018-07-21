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
            _starup = 1;
            _sort = 0;
        }

        // 名字
        std::string _name;

        // 是否启动
        uint32 _starup;

        // 排序
        uint32 _sort;

        // debug
        std::string _debug;

        // release
        std::string _release;

        // 配置路径
        std::string _config_file;
    };
    //////////////////////////////////////////////////////////////////////////////

    class KFNode;
    class KFAppConfig : public KFSingleton< KFAppConfig >
    {
    public:
        KFAppConfig();
        ~KFAppConfig();

        // 加载启动配置
        bool LoadStartupConfig( const char* file );


    protected:

        bool LoadServerConfig( const char* file );
        bool LoadCommonConfig( const char* file );

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

        // 服务器id
        uint32 _server_id;

        // 配置
        std::map< std::string, KFAppSetting > _startups;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _app_config = KFrame::KFAppConfig::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
