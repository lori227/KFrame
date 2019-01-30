#include "KFStartup.h"
#include "KFLibrary.h"
#include "KFAppConfig.h"

namespace KFrame
{
    bool KFStartup::InitStartup( const std::string& file )
    {
        // 加载配置
        if ( !_app_config->LoadStartupConfig( file ) )
        {
            return false;
        }

        return LoadPlugin();
    }

    bool KFStartup::LoadPlugin()
    {
        auto kfglobal = KFGlobal::Instance();

        for ( auto& kfsetting : _app_config->_startups )
        {
            auto loadplguin = kfsetting._name;
#ifdef __KF_DEBUG__
            loadplguin += "d";
#endif
            bool result = LoadPluginLibrary( loadplguin, &kfsetting );
            if ( !result )
            {
                return false;
            }
        }

        return true;
    }

    void KFStartup::ShutDown()
    {
        _kf_library.Clear();
    }

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage* manage, KFGlobal* kfglobal, KFMalloc* kfmalloc, KFLogger* kflogger );
    bool KFStartup::LoadPluginLibrary( const std::string& file, const KFAppSetting* kfsetting )
    {
        auto library = _kf_library.Create( kfsetting->_name );
        if ( !library->Load( _app_config->_plugin_path, file ) )
        {
            __LOG_ERROR__( "load [{}] failed!", library->_path );
            return false;
        }

        PluginEntryFunction function = ( PluginEntryFunction )library->GetFunction( "DllPluginEntry" );
        if ( function == nullptr )
        {
            __LOG_ERROR__( "entry [{}] failed!", library->_path );
            return false;
        }

        // 设置插件信息
        auto plugin = function( KFPluginManage::Instance(), KFGlobal::Instance(), KFMalloc::Instance(), KFLogger::Instance() );
        plugin->_sort = kfsetting->_sort;
        plugin->_plugin_name = kfsetting->_name;
        plugin->_config = kfsetting->_config_file;

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
        function( KFPluginManage::Instance() );

        library->UnLoad();
        _kf_library.Remove( pluginname );
        return true;
    }
}