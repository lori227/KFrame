#include "KFStartup.h"
#include "KFLibrary.h"
#include "KFAppConfig.h"

namespace KFrame
{
    KFStartup::KFStartup()
    {
    }

    KFStartup::~KFStartup()
    {

    }

    bool KFStartup::InitStartup( const char* file )
    {
        // 加载配置
        if ( !_app_config->LoadStartupConfig( file ) )
        {
            KFLogger::LogInit( KFLogger::Error, "load [%s] failed!", file );
            return false;
        }

        auto kfglobal = KFGlobal::Instance();
        kfglobal->_app_name = _app_config->_app_name;
        kfglobal->_app_type = _app_config->_app_type;

        return true;
    }

    bool KFStartup::LoadPlugin()
    {
        return LoadPluginLibrarys();
    }

    void KFStartup::ShutDown()
    {
        _kf_library.Clear();
    }

    bool KFStartup::LoadPluginLibrarys()
    {
        for ( auto iter = _app_config->_startups.begin(); iter != _app_config->_startups.end(); ++iter )
        {
            auto startupsetting = &iter->second;
            if ( startupsetting->_starup == 0 )
            {
                continue;
            }


#ifdef __KF_DEBUG__
            auto& loadplguin = startupsetting->_debug;
#else
            auto& loadplguin = startupsetting->_release;
#endif
            bool result = LoadPluginLibrary( loadplguin, startupsetting );
            if ( !result )
            {
                return false;
            }
        }

        return true;
    }

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage* manage, KFGlobal* kfglobal, KFMalloc* kfmalloc );

    static const std::string _plugin_path = "./plugin/";
    bool KFStartup::LoadPluginLibrary( const std::string& file, const KFAppSetting* appsetting )
    {
        auto kfglobal = KFGlobal::Instance();

        auto library = __KF_CREATE__( KFLibrary );
        if ( !library->Load( _plugin_path, file ) )
        {
            KFLogger::LogInit( KFLogger::Error, "load [%s] failed!", library->_path.c_str() );
            __KF_DESTROY__( KFLibrary, library );
            return false;
        }

        PluginEntryFunction function = ( PluginEntryFunction )library->GetFunction( "DllPluginEntry" );
        if ( function == nullptr )
        {
            KFLogger::LogInit( KFLogger::Error, "entry [%s] failed!", library->_path.c_str() );
            __KF_DESTROY__( KFLibrary, library );
            return false;
        }

        // 添加library
        _kf_library.Insert( appsetting->_name, library );

        // 设置插件信息
        auto plugin = function( _kf_plugin_manage, KFGlobal::Instance(), KFMalloc::Instance() );
        plugin->_sort = appsetting->_sort;
        plugin->_plugin_name = appsetting->_name;
        plugin->_config = appsetting->_config_file;

        KFLogger::LogInit( KFLogger::Info, "load [%s] ok!", library->_path.c_str() );
        return true;
    }

    typedef void( *PluginLeaveFunction )( KFPluginManage* manage );

    bool KFStartup::UnLoadPluginLibrary( const std::string& pluginname )
    {
        auto library = _kf_library.Find( pluginname );
        if ( library == nullptr )
        {
            return false;
        }

        PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
        if ( function != nullptr )
        {
            function( _kf_plugin_manage );
        }

        library->UnLoad();
        _kf_library.Remove( pluginname );
        return true;
    }
}