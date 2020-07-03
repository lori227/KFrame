#include "KFStartup.h"
#include "KFLibrary.h"
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
        for ( auto& kfsetting : _app_config->_startups )
        {
            auto kfplugin = LoadPluginLibrary( &kfsetting );
            if ( kfplugin == nullptr )
            {
                return false;
            }
        }

        return true;
    }

    typedef KFPlugin* ( *PluginEntryFunction )( KFPluginManage*, KFGlobal*, KFMalloc*, KFLogger* );
    KFPlugin* KFStartup::LoadPluginLibrary( const KFAppSetting* kfsetting )
    {
        auto library = _kf_library.Create( kfsetting->_name );

        auto file = kfsetting->GetName();
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
        auto kfplugin = function( KFPluginManage::Instance(), KFGlobal::Instance(), KFMalloc::Instance(), KFLogger::Instance() );
        kfplugin->_sort = kfsetting->_sort;
        kfplugin->_plugin_name = kfsetting->_name;
        kfplugin->_config = kfsetting->_config_file;

        __LOG_INFO__( "load [{}] ok", library->_path );
        return kfplugin;
    }

    typedef void( *PluginLeaveFunction )( KFPluginManage*, bool );

    bool KFStartup::UnLoadPluginLibrary( const std::string& pluginname )
    {
        auto library = _kf_library.Find( pluginname );
        if ( library == nullptr )
        {
            return false;
        }

        PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
        function( KFPluginManage::Instance(), true );

        library->UnLoad();
        return _kf_library.Remove( pluginname );
    }

    void KFStartup::ShutDown()
    {
        KFPluginManage::Instance()->UnRegisterCommandFunction( __STRING__( loadplugin ) );
        KFPluginManage::Instance()->ShutDown();

        for ( auto& iter : _kf_library._objects )
        {
            auto library = iter.second;
            PluginLeaveFunction function = ( PluginLeaveFunction )library->GetFunction( "DllPluginLeave" );
            function( KFPluginManage::Instance(), false );
        }
        _kf_library.Clear();
    }

    void KFStartup::ReloadPlugin( const StringVector& params )
    {
        if ( params.size() < 3u )
        {
            return __LOG_ERROR__( "reload param error" );
        }

        auto pluginname = params[ 0 ];
        auto kfsetting = _app_config->FindStartupSetting( pluginname );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "plugin=[{}] not setting", pluginname );
        }

        auto name = params[ 1 ];
        auto path = params[ 2 ];
        auto filename = __FORMAT__( "{}{}", path, name );
        auto newfilename = __FORMAT__( "{}.new", filename );

        try
        {
            // 判断新文件是否存在
            Poco::File newfile( newfilename );
            if ( !newfile.exists() )
            {
                return __LOG_ERROR__( "plugin=[{}] not exist", newfilename );
            }

        }
        catch ( ... )
        {
            __LOG_ERROR__( "plugin=[{}] init failed", filename );
        }

        __LOG_INFO__( "plugin=[{}] uninstall start", pluginname );

        // 卸载
        auto ok = UnLoadPluginLibrary( pluginname );
        if ( ok )
        {
            __LOG_INFO__( "plugin=[{}] uninstall ok", pluginname );
        }
        else
        {
            return __LOG_ERROR__( "plugin=[{}] uninstall failed", pluginname );
        }

        KFThread::Sleep( 200 );

        try
        {
            // 删除旧文件
            Poco::File oldfile( filename );
            oldfile.remove();
            __LOG_INFO__( "plugin=[{}] remove ok", filename );

            // 修改文件名
            Poco::File newfile( newfilename );
            newfile.copyTo( filename );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "plugin=[{}] rename failed", newfilename );
        }

        KFThread::Sleep( 200 );

        // 加载
        __LOG_INFO__( "plugin=[{}] install start", pluginname );
        auto kfplugin = LoadPluginLibrary( kfsetting );
        if ( kfplugin == nullptr )
        {
            return __LOG_INFO__( "plugin=[{}] install failed", pluginname );
        }

        // 重新load
        kfplugin->Install();
        KFPluginManage::Instance()->LoadModule();
        kfplugin->Reload();
        __LOG_INFO__( "plugin=[{}] install ok", pluginname );
    }
}