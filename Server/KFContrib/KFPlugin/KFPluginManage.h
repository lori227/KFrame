#ifndef __KF_PLUGIN_MANAGE_H__
#define __KF_PLUGIN_MANAGE_H__

#include "KFrame.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_PLUGIN_ENRTY__( classname )\
    KFrame::KFPlugin* DllPluginEntry( KFrame::KFPluginManage* pluginmanage, KFrame::KFGlobal* kfglobal, KFrame::KFMalloc* kfmalloc )\
    {\
        KFrame::KFGlobal::Initialize( kfglobal );\
        KFrame::KFMalloc::Initialize( kfmalloc );\
        return pluginmanage->RegistPlugin< classname >();\
    }\

#define __KF_PLUGIN_LEAVE__( classname ) \
    void DllPluginLeave( KFrame::KFPluginManage* pluginmanage )\
    {\
        pluginmanage->UnRegistPlugin< classname >();\
    }\

#if __KF_SYSTEM__ == __KF_WIN__
#define __CHECK_PLUGIN_FUNCTION__( name, function )\
    if ( &KFModule::function != &name##Module::function )\

#else
#define __CHECK_PLUGIN_FUNCTION__( name, function )\
    KFModule kfbase;\
    void ( KFModule::*basemfp )() = &KFModule::function; \
    auto bassaddress = ( void* )( kfbase.*basemfp ); \
    auto kfmodule = static_cast < name##Module* >( _kf_module );\
    void ( name##Module::*childmfp )() = &name##Module::function; \
    auto childaddress = (void*)( kfmodule->*childmfp );\
    if ( bassaddress != childaddress )\

#endif

#define __REGISTER_PLUGIN_FUNCTION__( name, function )\
    {\
        __CHECK_PLUGIN_FUNCTION__( name, function )\
        {   \
            _kf_plugin_manage->Register##function##Function< name##Module >( _sort, kfmodule, &name##Module::function );\
        }\
    }

#define __UNREGISTER_PLUGIN_FUNCTION__( name, function )\
    {\
        __CHECK_PLUGIN_FUNCTION__( name, function )\
        {   \
            _kf_plugin_manage->UnRegister##function##Function( _sort );\
        }\
    }

// 注册模块
#define __REGISTER_MODULE__( name ) \
    auto kfmodule = new name##Module();\
    _kf_plugin_manage->RegistModule< name##Plugin, name##Interface >( kfmodule );\
    __REGISTER_PLUGIN_FUNCTION__( name, Run );\
    __REGISTER_PLUGIN_FUNCTION__( name, AfterRun );\

// 卸载模块
#define __UNREGISTER_MODULE__( name ) \
    __UNREGISTER_PLUGIN_FUNCTION__( name, Run );\
    __UNREGISTER_PLUGIN_FUNCTION__( name, AfterRun );\
    _kf_plugin_manage->UnRegistModule< name##Plugin >();\

#define __FIND_MODULE__( module, classname ) \
    module = _kf_plugin_manage->FindModule< classname >( __FILE__, __LINE__ )

#define __FIND_MODULE_NO_LOG__( module, classname ) \
    module = _kf_plugin_manage->FindModule< classname >()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    class KFDebugCommand
    {
    public:
        std::string _command;
    };

    typedef std::function<void( const VectorString& )> KFCmdFunction;
    typedef std::function<void()> KFRunFunction;

    //////////////////////////////////////////////////////////////////////
    class KFAppSetting;
    class KFPluginManage : public KFSingleton< KFPluginManage >
    {
    public:

        KFPluginManage();
        ~KFPluginManage();

        ///////////////////////////////////////////////////////////////////////////
        // 初始化
        void InitPlugin();

        // 关闭
        void ShutDown();

        // 逻辑
        void Run();

        //
        void AddCommand( const std::string& command );
        /////////////////////////////////////////////////////////////////////////

        // 注册插件
        template< class T >
        T* RegistPlugin()
        {
            auto plugin = new T ();
            return dynamic_cast<T*>( RegistPlugin( typeid( T ).name(), plugin ) );
        }

        // 卸载插件
        template< class T >
        void UnRegistPlugin()
        {
            std::string name = typeid( T ).name();
            UnRegistPlugin( name );
        }

        // 查找插件
        template< class T >
        T* FindPlugin()
        {
            std::string name = typeid( T ).name();
            return dynamic_cast<T*>( FindPlugin( name ) );
        }

        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        // 注册模块
        template< class PluginType, class InterfaceType >
        void RegistModule( InterfaceType* module )
        {
            auto plugin = FindPlugin< PluginType >();
            plugin->BindModule( typeid( InterfaceType ).name(), module );
        }

        // 卸载模块
        template< class PluginType >
        void UnRegistModule()
        {
            auto plugin = FindPlugin< PluginType >();
            plugin->UnBindModule();
        }

        // 查找模块
        template< class InterfaceType >
        InterfaceType* FindModule( const char* file, uint32 line )
        {
            std::string name = typeid( InterfaceType ).name();
            return dynamic_cast< InterfaceType*>( FindModule( name, file, line ) );
        }

        template< class InterfaceType >
        InterfaceType* FindModule()
        {
            std::string name = typeid( InterfaceType ).name();
            return dynamic_cast< InterfaceType* >( FindModule( name ) );
        }
        /////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterCommandFunction( const std::string& command, T* object, void ( T::*handle )( const VectorString& ) )
        {
            auto kffunction = _command_functions.Create( command );
            kffunction->_function = std::bind( handle, object, std::placeholders::_1 );
        }

        void UnRegisterCommandFunction( const std::string& command )
        {
            _command_functions.Remove( command );
        }

        bool ProcessCommand( std::string& cmd, VectorString& param, std::string split = "," );

        /////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterRunFunction( uint32 sort, T* object, void ( T::*handle )() )
        {
            auto kffunction = _run_functions.Find( sort );
            if ( kffunction != nullptr )
            {
                return __LOG_ERROR__( "sort[{}] run is already register!", sort );
            }

            kffunction = _run_functions.Create( sort );
            kffunction->_function = std::bind( handle, object );
        }

        void UnRegisterRunFunction( uint32 sort )
        {
            _run_functions.Remove( sort );
        }

        /////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterAfterRunFunction( uint32 sort, T* object, void ( T::*handle )( ) )
        {
            auto kffunction = _after_run_functions.Find( sort );
            if ( kffunction != nullptr )
            {
                return __LOG_ERROR__( "sort[{}] afterrun is already register!", sort );
            }

            kffunction = _after_run_functions.Create( sort );
            kffunction->_function = std::bind( handle, object );
        }

        void UnRegisterAfterRunFunction( uint32 sort )
        {
            _after_run_functions.Remove( sort );
        }
        /////////////////////////////////////////////////////////////////

    protected:
        ////////////////////////////////////////////////////////////////////
        // 安装模块
        void InstallPlugin();

        // 加载模块
        void LoadModule();

        // 初始化模块
        void InitModule();

        // 加载配置
        void LoadConfig();

        // 准备执行
        void BeforeRun();

        // 执行一次
        void OnceRun();

        // 执行命令
        void RunCommand();
        ////////////////////////////////////////////////////////////////////

        // 注册插件
        KFPlugin* RegistPlugin( const std::string& name, KFPlugin* plugin );

        // 卸载插件
        void UnRegistPlugin( const std::string& name );

        // 查找插件
        KFPlugin* FindPlugin( const std::string& name );

        // 查找模块
        KFModule* FindModule( const std::string& name, const char* file, uint32 line );
        KFModule* FindModule( const std::string& name );

    protected:
        std::vector< KFPlugin* > _plugins;

        //
        KFQueue< KFDebugCommand > _commands;
        KFBind< std::string, const std::string&, KFCmdFunction > _command_functions;

        // 注册的Run函数
        KFBind< uint32, uint32, KFRunFunction > _run_functions;
        KFBind< uint32, uint32, KFRunFunction > _after_run_functions;
    };
}

#endif