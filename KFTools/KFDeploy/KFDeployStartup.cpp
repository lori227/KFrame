#include "KFDeployStartup.h"
#include "KFDeployConfig.h"

namespace KFrame
{
    bool KFDeployStartup::InitStartup( const std::string& file )
    {
        // 加载配置
        if ( !_deploy_config->LoadStartupConfig( file ) )
        {
            return false;
        }

        return LoadPlugin();
    }

    bool KFDeployStartup::LoadPlugin()
    {
        auto global = KFGlobal::Instance();

        for ( auto& setting : _deploy_config->_startups )
        {
            auto loadplguin = setting._name;
#ifdef __KF_DEBUG__
            loadplguin += "d";
#endif
            bool result = LoadPluginLibrary( loadplguin, &setting );
            if ( !result )
            {
                return false;
            }
        }

        return true;
    }

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage* manage, KFGlobal* global, KFMalloc* kfmalloc, KFLogger* kflogger );
    bool KFDeployStartup::LoadPluginLibrary( const std::string& file, const KFDeploySetting* setting )
    {
        auto library = _kf_library.Create( setting->_name );
        if ( !library->Load( _deploy_config->_plugin_path, file ) )
        {
            __LOG_ERROR__( "load [{}] failed", library->_path );
            return false;
        }

        PluginEntryFunction function = ( PluginEntryFunction )library->GetFunction( "DllPluginEntry" );
        if ( function == nullptr )
        {
            __LOG_ERROR__( "entry [{}] failed", library->_path );
            return false;
        }

        // 设置插件信息
        auto plugin = function( KFPluginManage::Instance(), KFGlobal::Instance(), KFMalloc::Instance(), KFLogger::Instance() );
        plugin->_sort = setting->_sort;
        plugin->_plugin_name = setting->_name;
        plugin->_config = setting->_config_file;

        __LOG_INFO__( "load [{}] ok", library->_path );
        return true;
    }

    typedef void( *PluginLeaveFunction )( KFPluginManage* manage );

    bool KFDeployStartup::UnLoadPluginLibrary( const std::string& pluginname )
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


    void KFDeployStartup::ShutDown()
    {
        for ( auto& iter : _kf_library._objects )
        {
            auto library = iter.second;
            PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
            function( KFPluginManage::Instance() );
        }

        _kf_library.Clear();
    }
}