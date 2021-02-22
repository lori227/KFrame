#include "KFStartup.h"
#include "KFAppConfig.h"
#include "Poco/File.h"

namespace KFrame
{
    bool KFStartup::InitStartup( const std::string& file )
    {
        KFPluginManage::Instance()->RegisterCommandFunction( __STRING__( loadplugin ), this, &KFStartup::ReloadPlugin );

        // 加载配置
        if ( !_app_config->LoadStartupConfig( file ) )
        {
            __LOG_ERROR__( "load [{}] failed", file );
            return false;
        }

        if ( !LoadPlugin() )
        {
            return false;
        }

        return KFPluginManage::Instance()->InitPlugin();
    }

    bool KFStartup::LoadPlugin()
    {
        for ( auto& setting : _app_config->_startups )
        {
            auto plugin = LoadPluginLibrary( &setting );
            if ( plugin == nullptr )
            {
                return false;
            }
        }

        return true;
    }

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage*, KFGlobal*, KFMalloc*, KFLogger* );
    KFPlugin* KFStartup::LoadPluginLibrary( const KFAppSetting* setting )
    {
        auto library = _library_list.Create( setting->_name );

        auto file = setting->GetName();
        if ( !library->Load( _app_config->_plugin_path, file ) )
        {
            __LOG_ERROR__( "load [{}] failed", library->_path );
            return nullptr;
        }

        PluginEntryFunction function = ( PluginEntryFunction )library->GetFunction( "DllPluginEntry" );
        if ( function == nullptr )
        {
            __LOG_ERROR__( "entry [{}] failed", library->_path );
            return nullptr;
        }

        // 设置插件信息
        auto plugin = function( KFPluginManage::Instance(), KFGlobal::Instance(), KFMalloc::Instance(), KFLogger::Instance() );
        plugin->_sort = setting->_sort;
        plugin->_param = setting->_param;
        plugin->_plugin_name = setting->_name;

        __LOG_INFO__( "load [{}] ok", library->_path );
        return plugin;
    }

    typedef void( *PluginLeaveFunction )( KFPluginManage*, bool );
    bool KFStartup::UnLoadPluginLibrary( const std::string& plugin_name )
    {
        auto library = _library_list.Remove( plugin_name );
        if ( library != nullptr )
        {
            PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
            function( KFPluginManage::Instance(), true );

            library->UnLoad();
        }

        return library != nullptr;
    }

    void KFStartup::ShutDown()
    {
        KFPluginManage::Instance()->UnRegisterCommandFunction( __STRING__( loadplugin ) );
        KFPluginManage::Instance()->ShutDown();

        for ( auto& iter : _library_list._objects )
        {
            auto library = iter.second;
            PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
            function( KFPluginManage::Instance(), false );
        }
        _library_list.Clear();
    }

    void KFStartup::ReloadPlugin( const StringVector& params )
    {
        if ( params.size() < 3u )
        {
            return __LOG_ERROR__( "reload param error" );
        }

        auto plugin_name = params[ 0 ];
        auto setting = _app_config->FindStartupSetting( plugin_name );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "plugin=[{}] not setting", plugin_name );
        }

        auto name = params[ 1 ];
        auto path = params[ 2 ];
        auto file_name = __FORMAT__( "{}{}", path, name );
        auto new_file_name = __FORMAT__( "{}.new", file_name );

        try
        {
            // 判断新文件是否存在
            Poco::File new_file( new_file_name );
            if ( !new_file.exists() )
            {
                return __LOG_ERROR__( "plugin=[{}] not exist", new_file_name );
            }
        }
        catch ( ... )
        {
            __LOG_ERROR__( "plugin=[{}] init failed", file_name );
        }

        __LOG_INFO__( "plugin=[{}] uninstall start", plugin_name );

        // 卸载
        auto ok = UnLoadPluginLibrary( plugin_name );
        if ( ok )
        {
            __LOG_INFO__( "plugin=[{}] uninstall ok", plugin_name );
        }
        else
        {
            return __LOG_ERROR__( "plugin=[{}] uninstall failed", plugin_name );
        }

        KFThread::Sleep( 100 );

        try
        {
            // 删除旧文件
            Poco::File old_file( file_name );
            old_file.remove();
            __LOG_INFO__( "plugin=[{}] remove ok", file_name );

            // 拷贝新文件
            Poco::File new_file( new_file_name );
            new_file.copyTo( file_name );

            // 删除新文件
            new_file.remove();
            __LOG_INFO__( "plugin=[{}] copy ok", file_name );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "plugin=[{}] rename failed", new_file_name );
        }

        KFThread::Sleep( 100 );

        // 加载
        __LOG_INFO__( "plugin=[{}] install start", plugin_name );
        auto plugin = LoadPluginLibrary( setting );
        if ( plugin == nullptr )
        {
            return __LOG_INFO__( "plugin=[{}] install failed", plugin_name );
        }

        // 重新load
        KFPluginManage::Instance()->ReloadPlugin( plugin );
        __LOG_INFO__( "plugin=[{}] install ok", plugin_name );
    }
}