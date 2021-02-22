#include "KFPluginManage.h"

namespace KFrame
{
    KFPluginManage::KFPluginManage()
    {
        _commands.InitQueue( 100u, 100u );
    }

    KFPluginManage::~KFPluginManage()
    {
        for ( auto plugin : _plugins )
        {
            __DELETE_OBJECT__( plugin );
        }
        _plugins.clear();
    }

    KFPlugin* KFPluginManage::RegisterPlugin( const std::string& name, KFPlugin* plugin )
    {
        UnRegisterPlugin( name, false );

        plugin->_class_name = name;
        plugin->_kf_plugin_manage = this;
        _plugins.push_back( plugin );
        return plugin;
    }

    void KFPluginManage::UnRegisterPlugin( const std::string& name, bool save_data )
    {
        for ( auto iter = _plugins.begin(); iter != _plugins.end(); ++iter )
        {
            auto plugin = *iter;
            if ( plugin->_class_name == name )
            {
                plugin->_save_data = save_data;
                plugin->UnInstall();

                __DELETE_OBJECT__( plugin );
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
        auto module = FindModule( name );
        if ( module == nullptr )
        {
            static std::string error = __FORMAT__( "[{}:{}] can't find [{}] module", file, line, name );
            throw std::runtime_error( error );
        }

        return module;
    }
    ///////////////////////////////////////////////////////////////
    void KFPluginManage::LoadModule()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->LoadModule();
        }
    }

    void KFPluginManage::InitModule()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->InitModule();
        }
    }

    void KFPluginManage::AddConfig()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->AddConfig();
        }
    }

    void KFPluginManage::LoadConfig()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->LoadConfig();
        }
    }

    void KFPluginManage::AfterLoad()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->AfterLoad();
        }
    }

    void KFPluginManage::BeforeRun()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->BeforeRun();
        }
    }

    void KFPluginManage::PrepareRun()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->PrepareRun();
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

            // 添加配置
            AddConfig();

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
            if ( _init_plugin->_init_module == nullptr )
            {
                __LOG_ERROR__( "plugin=[{}] init exception=[{}]", _init_plugin->_plugin_name, ex.what() );
            }
            else
            {
                __LOG_ERROR__( "plugin=[{}] module=[{}] init exception=[{}]", _init_plugin->_plugin_name, _init_plugin->_init_module->_class_name, ex.what() );
            }

        }
        catch ( ... )
        {
            if ( _init_plugin->_init_module == nullptr )
            {
                __LOG_ERROR__( "plugin=[{}] init exception unknown", _init_plugin->_plugin_name );
            }
            else
            {
                __LOG_ERROR__( "plugin=[{}] module=[{}] init exception unknown", _init_plugin->_plugin_name, _init_plugin->_init_module->_class_name );
            }
        }

        return false;
    }

    void KFPluginManage::ReloadPlugin( KFPlugin* plugin )
    {
        plugin->Reload();

        // 重新初始化插件
        LoadModule();
    }

    void KFPluginManage::ShutDown()
    {
        for ( auto plugin : _plugins )
        {
            plugin->BeforeShut();
        }

        for ( auto plugin : _plugins )
        {
            plugin->ShutDown();
        }

        for ( auto plugin : _plugins )
        {
            plugin->AfterShut();
        }
    }

    void KFPluginManage::Run()
    {
        RunCommand();

        // 逻辑run
        for ( auto& iter : _run_functions._objects )
        {
            iter.second->Call();
        }

        // 延迟执行的逻辑
        for ( auto& iter : _after_run_functions._objects )
        {
            iter.second->Call();
        }
    }

    ///////////////////////////////////////////////////////////////
    template <class T>
    struct PluginLesser : std::binary_function<T, T, bool>
    {
        bool operator() ( const T* x, const T* y ) const
        {
            return x->_sort < y->_sort;
        }
    };

    void KFPluginManage::InstallPlugin()
    {
        for ( auto plugin : _plugins )
        {
            _init_plugin = plugin;
            plugin->Install();
        }

        // 排序
        std::sort( _plugins.begin(), _plugins.end(), PluginLesser<KFPlugin>() );
    }

    ///////////////////////////////////////////////////////////////
    void KFPluginManage::AddCommand( std::string command )
    {
        if ( command.empty() )
        {
            return;
        }

        static std::string _split = "=";

        auto command_data = __MAKE_SHARED__( KFDebugCommand );
        command_data->_command = KFUtility::SplitString( command, _split );
        while ( !command.empty() )
        {
            auto param_value = KFUtility::SplitString( command, _split );
            if ( !param_value.empty() )
            {
                command_data->_params.push_back( param_value );
            }
        }

        _commands.Push( command_data );
    }

    void KFPluginManage::RunCommand()
    {
        auto command_data = _commands.Pop();
        while ( command_data != nullptr )
        {
            auto function = _command_functions.Find( command_data->_command );
            if ( function != nullptr )
            {
                function->Call( command_data->_params );
            }

            command_data = _commands.Pop();
        }
    }

    void KFPluginManage::SetModuleOpen( const std::string& module_name, bool is_open )
    {
        for ( auto plugin : _plugins )
        {
            for ( auto& iter : plugin->_modules )
            {
                auto module = iter.second;
                if ( module->_plugin_name == module_name )
                {
                    module->_is_open = is_open;
                    return;
                }
            }
        }
    }
}