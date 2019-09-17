#include "KFPluginManage.h"

namespace KFrame
{
    KFPluginManage::KFPluginManage()
    {
        _commands.InitQueue( 100 );
    }

    KFPluginManage::~KFPluginManage()
    {
        for ( auto plugin : _plugins )
        {
            delete plugin;
        }
        _plugins.clear();
    }

    KFPlugin* KFPluginManage::RegistPlugin( const std::string& name, KFPlugin* plugin )
    {
        UnRegistPlugin( name, false );

        plugin->_class_name = name;
        plugin->_kf_plugin_manage = this;
        _plugins.push_back( plugin );
        return plugin;
    }

    void KFPluginManage::UnRegistPlugin( const std::string& name, bool savedata )
    {
        for ( auto iter = _plugins.begin(); iter != _plugins.end(); ++iter )
        {
            auto plugin = *iter;
            if ( plugin->_class_name == name )
            {
                plugin->_save_data = savedata;
                plugin->UnInstall();

                delete plugin;
                _plugins.erase( iter );
                break;
            }
        }
    }

    KFPlugin* KFPluginManage::FindPlugin( const std::string& name )
    {
        for ( auto plugin : _plugins )
        {
            if ( plugin->_class_name == name )
            {
                return plugin;
            }
        }

        return nullptr;
    }

    KFModule* KFPluginManage::FindModule( const std::string& name )
    {
        for ( auto plugin : _plugins )
        {
            auto module = plugin->FindModule( name );
            if ( module != nullptr )
            {
                return module;
            }
        }

        return nullptr;
    }

    KFModule* KFPluginManage::FindModule( const std::string& name, const char* file, uint32 line )
    {
        auto kfmodule = FindModule( name );
        if ( kfmodule == nullptr )
        {
            throw std::runtime_error( __FORMAT__( "[{}:{}] can't find [{}] module!", file, line, name ) );
        }

        return kfmodule;
    }
    ///////////////////////////////////////////////////////////////
    void KFPluginManage::LoadModule()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->LoadModule();
        }
    }

    void KFPluginManage::InitModule()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->InitModule();
        }
    }

    void KFPluginManage::LoadConfig()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->AddConfig();
        }

        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->LoadConfig();
        }
    }

    void KFPluginManage::AfterLoad()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->AfterLoad();
        }
    }

    void KFPluginManage::BeforeRun()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->BeforeRun();
        }
    }

    void KFPluginManage::PrepareRun()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->PrepareRun();
        }
    }

    bool KFPluginManage::InitPlugin()
    {
        try
        {
            // 安装插件
            InstallPlugin();

            // 加载模块
            LoadModule();

            // 初始化
            InitModule();

            // 加载配置
            LoadConfig();

            // 加载完成
            AfterLoad();

            // 准备运行
            BeforeRun();

            // 执行一次
            PrepareRun();

            return true;
        }
        catch ( std::exception& ex )
        {
            __LOG_ERROR__( "plugin=[{}] module=[{}] init exception=[{}]!",
                           _init_plugin->_plugin_name, _init_plugin->_init_module->_class_name,
                           ex.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "plugin=[{}] module=[{}] init exception unknown!",
                           _init_plugin->_plugin_name, _init_plugin->_init_module->_class_name );
        }

        return false;
    }

    void KFPluginManage::ShutDown()
    {
        for ( auto kfplugin : _plugins )
        {
            kfplugin->BeforeShut();
        }

        for ( auto kfplugin : _plugins )
        {
            kfplugin->ShutDown();
        }

        for ( auto kfplugin : _plugins )
        {
            kfplugin->AfterShut();
        }
    }

    void KFPluginManage::Run()
    {
        RunCommand();

        // 逻辑run
        for ( auto& iter : _run_functions._objects )
        {
            iter.second->_function();
        }

        // 延迟执行的逻辑
        for ( auto& iter : _after_run_functions._objects )
        {
            iter.second->_function();
        }
    }

    ///////////////////////////////////////////////////////////////
    template <class T>
    struct PluginLesser : std::binary_function <T, T, bool>
    {
        bool operator() ( const T* x, const T* y ) const
        {
            return x->_sort < y->_sort;
        }
    };

    void KFPluginManage::InstallPlugin()
    {
        for ( auto kfplugin : _plugins )
        {
            _init_plugin = kfplugin;
            kfplugin->Install();
        }

        // 排序
        std::sort( _plugins.begin(), _plugins.end(), PluginLesser< KFPlugin >() );
    }

    ///////////////////////////////////////////////////////////////
    void KFPluginManage::AddCommand( std::string command )
    {
        if ( command.empty() )
        {
            return;
        }

        static std::string _split = "=";

        auto kfcommand = __KF_NEW__( KFDebugCommand );
        kfcommand->_command = KFUtility::SplitString( command, _split );
        while ( !command.empty() )
        {
            auto paramvalue = KFUtility::SplitString( command, _split );
            if ( !paramvalue.empty() )
            {
                kfcommand->_params.push_back( paramvalue );
            }
        }

        _commands.PushObject( kfcommand );
    }

    void KFPluginManage::RunCommand()
    {
        auto kfcommand = _commands.PopObject();
        while ( kfcommand != nullptr )
        {
            auto kffunction = _command_functions.Find( kfcommand->_command );
            if ( kffunction != nullptr )
            {
                kffunction->_function( kfcommand->_params );
            }

            __KF_DELETE__( KFDebugCommand, kfcommand );
            kfcommand = _commands.PopObject();
        }
    }
}