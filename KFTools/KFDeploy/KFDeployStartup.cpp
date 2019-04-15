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
        auto kfglobal = KFGlobal::Instance();

        for ( auto& kfsetting : _deploy_config->_startups )
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

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage* manage, KFGlobal* kfglobal, KFMalloc* kfmalloc, KFLogger* kflogger );
    bool KFDeployStartup::LoadPluginLibrary( const std::string& file, const KFDeploySetting* kfsetting )
    {
        auto library = _kf_library.Create( kfsetting->_name );
        if ( !library->Load( _deploy_config->_plugin_path, file ) )
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

        __LOG_INFO__( "load [{}] ok!", library->_path );
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