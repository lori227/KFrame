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
        UnRegistPlugin( name );

        plugin->_class_name = name;
        plugin->_kf_plugin_manage = this;
        _plugins.push_back( plugin );

        plugin->Install();
        return plugin;
    }

    void KFPluginManage::UnRegistPlugin( const std::string& name )
    {
        for ( auto iter = _plugins.begin(); iter != _plugins.end(); ++iter )
        {
            auto plugin = *iter;
            if ( plugin->_class_name == name )
            {
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

    KFModule* KFPluginManage::FindModule( const std::string& name, const char* file, uint32 line )
    {
        for ( auto plugin : _plugins )
        {
            auto module = plugin->FindModule( name );
            if ( module != nullptr )
            {
                return module;
            }
        }

        __LOG_ERROR_FUNCTION__( KFLogEnum::Init, file, line, "can't find [{}] module!", name );
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////
    void KFPluginManage::LoadModule()
    {
        for ( auto iter : _plugins )
        {
            iter->LoadModule();
        }
    }

    void KFPluginManage::InitModule()
    {
        for ( auto iter : _plugins )
        {
            iter->InitModule();
        }
    }

    void KFPluginManage::LoadConfig()
    {
        for ( auto iter : _plugins )
        {
            iter->LoadConfig();
        }

        for ( auto iter : _plugins )
        {
            iter->AfterLoad();
        }
    }

    void KFPluginManage::BeforeRun()
    {
        for ( auto iter : _plugins )
        {
            iter->BeforeRun();
        }
    }

    void KFPluginManage::OnceRun()
    {
        for ( auto iter : _plugins )
        {
            iter->OnceRun();
        }
    }

    void KFPluginManage::InitPlugin()
    {
        SortPlugin();

        // 加载模块
        LoadModule();

        // 初始化
        InitModule();

        // 加载配置
        LoadConfig();

        // 准备运行
        BeforeRun();

        // 执行一次
        OnceRun();
    }

    void KFPluginManage::ShutDown()
    {
        for ( auto& iter : _plugins )
        {
            iter->BeforeShut();
        }

        for ( auto& iter : _plugins )
        {
            iter->ShutDown();
        }

        for ( auto& iter : _plugins )
        {
            iter->AfterShut();
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

    void KFPluginManage::SortPlugin()
    {
        std::sort( _plugins.begin(), _plugins.end(), PluginLesser< KFPlugin >() );
    }

    ///////////////////////////////////////////////////////////////
    void KFPluginManage::AddCommand( const std::string& command )
    {
        auto kfcommand = __KF_CREATE__( KFDebugCommand );
        kfcommand->_command = command;
        _commands.PushObject( kfcommand );
    }

    void KFPluginManage::RunCommand()
    {
        auto kfcommand = _commands.PopObject();
        while ( kfcommand != nullptr )
        {
            // 解析
            VectorString params;
            this->ProcessCommand( kfcommand->_command, params );
            if ( params.size() >= 2 )
            {
                auto kffunction = _command_functions.Find( params[ 0 ] );
                if ( kffunction != nullptr )
                {
                    kffunction->_function( params );
                }
            }
            __KF_DESTROY__( KFDebugCommand, kfcommand );
            kfcommand = _commands.PopObject();
        }
    }

    bool KFPluginManage::ProcessCommand( std::string& cmd, VectorString& param, std::string split /* = " */ )
    {
        if ( cmd.empty() )
        {
            // replace log4cxx
            std::cout << "param is empty" << std::endl;
            return false;
        }

        auto key = KFUtility::SplitString( cmd, split );
        if ( key.empty() )
        {
            // replace log4cxx
            std::cout << "key is empty" << std::endl;
            return false;
        }
        param.push_back( key );
        auto paramvalue = KFUtility::SplitString( cmd, split );
        auto looptime = 0;

        while ( !paramvalue.empty() )
        {
            if ( looptime >= 10 )
            {
                // replace log4cxx
                std::cout << "parse param out of range" << std::endl;
                return false;
            }
            param.push_back( paramvalue );
            paramvalue = KFUtility::SplitString( cmd, split );
            looptime++;

        }
        return true;
    }

}