#ifndef __KF_PLUGIN_MANAGE_H__
#define __KF_PLUGIN_MANAGE_H__

#include "KFrame.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_PLUGIN_ENTRY__( class_name )\
    KFrame::KFPlugin* DllPluginEntry( KFrame::KFPluginManage* plugin_manage, KFrame::KFGlobal* global_data, KFrame::KFMalloc* malloc_data, KFrame::KFLogger* logger_data )\
    {\
        KFrame::KFGlobal::Initialize( global_data );\
        KFrame::KFMalloc::Initialize( malloc_data );\
        KFrame::KFLogger::Initialize( logger_data );\
        return plugin_manage->RegisterPlugin<class_name>();\
    }\

#define __KF_PLUGIN_LEAVE__( class_name ) \
    void DllPluginLeave( KFrame::KFPluginManage* plugin_manage, bool save_data )\
    {\
        plugin_manage->UnRegisterPlugin<class_name>( save_data );\
    }\

#if __KF_SYSTEM__ == __KF_WIN__
#define __CHECK_PLUGIN_FUNCTION__( module_name, function )\
    std::string base_function_name = typeid( &KFModule::function ).name();\
    std::string child_function_name = typeid( &module_name::function ).name();\
    if ( base_function_name != child_function_name )\

#else
#define __CHECK_PLUGIN_FUNCTION__( module_name, function )\
    KFModule base_module;\
    void ( KFModule::*base_mfp )() = &KFModule::function; \
    auto bass_address = (void*)( base_module.*base_mfp ); \
    void ( module_name::*child_mfp )() = &module_name::function; \
    auto child_address = (void*)( module->*child_mfp );\
    if ( bass_address != child_address )\

#endif

#define __REGISTER_PLUGIN_FUNCTION__( module_name, function )\
    {\
        __CHECK_PLUGIN_FUNCTION__( module_name, function )\
        {   \
            auto ok = _kf_plugin_manage->Register##function##Function<module_name>( _sort, module, &module_name::function );\
            if ( ok )\
            {\
                __LOG_INFO__( "module=[{}] sort=[{}] function=[{}] register ok", #module_name, _sort, #function );\
            }\
            else\
            {\
                __LOG_ERROR__( "module=[{}] sort=[{}] function=[{}] register failed", #module_name, _sort, #function );\
            }\
        }\
    }

#define __UN_PLUGIN_FUNCTION__( module_name, function )\
    {\
        __CHECK_PLUGIN_FUNCTION__( module_name, function )\
        {   \
            _kf_plugin_manage->UnRegister##function##Function( _sort );\
        }\
    }

// 注册模块
#define __REGISTER_MODULE__( module_name ) \
    {\
        auto module = __NEW_OBJECT__( module_name##Module ); \
        module->_plugin_name = #module_name;\
        _kf_plugin_manage->RegisterModule<module_name##Interface>( this, module );\
        __REGISTER_PLUGIN_FUNCTION__( module_name##Module, Run );\
        __REGISTER_PLUGIN_FUNCTION__( module_name##Module, AfterRun );\
    }\

// 卸载模块
#define __UN_MODULE__( module_name ) \
    {\
        auto module = (module_name##Module*)FindModule( typeid( module_name##Interface ).name());\
        __UN_PLUGIN_FUNCTION__( module_name##Module, Run ); \
        __UN_PLUGIN_FUNCTION__( module_name##Module, AfterRun );\
        _kf_plugin_manage->UnRegisterModule<module_name##Interface>( this, _save_data );\
        __DELETE_OBJECT__( module );\
    }\

#define __FIND_MODULE__( module, class_name ) \
    module = _kf_plugin_manage->FindModule<class_name>( __FILE__, __LINE__ )

#define __FIND_MODULE_NO_LOG__( module, class_name ) \
    module = _kf_plugin_manage->FindModule<class_name>()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    class KFDebugCommand
    {
    public:
        std::string _command;
        StringVector  _params;
    };

    typedef std::function<void()> KFRunFunction;
    typedef std::function<void( const StringVector& )> KFCmdFunction;
    //////////////////////////////////////////////////////////////////////
    class KFAppSetting;
    class KFPluginManage : public KFSingleton< KFPluginManage >
    {
    public:
        KFPluginManage();
        ~KFPluginManage();

        ///////////////////////////////////////////////////////////////////////////
        // 初始化
        bool InitPlugin();

        // 重新加载插件
        void ReloadPlugin( KFPlugin* plugin );

        // 关闭
        void ShutDown();

        // 逻辑
        void Run();

        // 添加命令
        void AddCommand( std::string command );

        // 加载模块
        void LoadModule();

        // 加载完成
        void AfterLoad();

        // 设置模块开关
        void SetModuleOpen( const std::string& module_name, bool is_open );
        /////////////////////////////////////////////////////////////////////////

        // 注册插件
        template<class T>
        T* RegisterPlugin()
        {
            auto plugin = __NEW_OBJECT__( T );
            return dynamic_cast<T*>( RegisterPlugin( typeid( T ).name(), plugin ) );
        }

        // 卸载插件
        template<class T>
        void UnRegisterPlugin( bool save_data )
        {
            std::string name = typeid( T ).name();
            UnRegisterPlugin( name, save_data );
        }

        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        // 注册模块
        template< class InterfaceType >
        void RegisterModule( KFPlugin* plugin, InterfaceType* module )
        {
            plugin->BindModule( typeid( InterfaceType ).name(), module );
        }

        // 卸载模块
        template< class InterfaceType >
        void UnRegisterModule( KFPlugin* plugin, bool save_data )
        {
            plugin->UnBindModule( typeid( InterfaceType ).name(), save_data );
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
        template<class T>
        void RegisterCommandFunction( const std::string& command, T* module, void ( T::*handle )( const StringVector& ) )
        {
            auto function = _command_functions.Create( command );
            function->SetFunction( std::bind( handle, module, std::placeholders::_1 ) );
        }

        void UnRegisterCommandFunction( const std::string& command )
        {
            _command_functions.Remove( command );
        }

        /////////////////////////////////////////////////////////////////
        template<class T>
        bool RegisterRunFunction( uint32 sort, T* module, void ( T::*handle )() )
        {
            auto function = _run_functions.Find( sort );
            if ( function != nullptr )
            {
                return false;
            }

            function = _run_functions.Create( sort );
            function->SetFunction( module, std::bind( handle, module ) );
            return true;
        }

        void UnRegisterRunFunction( uint32 sort )
        {
            _run_functions.Remove( sort );
        }

        /////////////////////////////////////////////////////////////////
        template<class T>
        bool RegisterAfterRunFunction( uint32 sort, T* module, void ( T::*handle )( ) )
        {
            auto function = _after_run_functions.Find( sort );
            if ( function != nullptr )
            {
                return false;
            }

            function = _after_run_functions.Create( sort );
            function->SetFunction( module, std::bind( handle, module ) );
            return true;
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

        // 初始化模块
        void InitModule();

        // 添加配置
        void AddConfig();

        // 加载配置
        void LoadConfig();

    protected:
        // 准备执行
        void BeforeRun();

        // 执行一次
        void PrepareRun();

        ////////////////////////////////////////////////////////////////////
        // 执行命令
        void RunCommand();
        ////////////////////////////////////////////////////////////////////
        // 注册插件
        KFPlugin* RegisterPlugin( const std::string& name, KFPlugin* plugin );

        // 卸载插件
        void UnRegisterPlugin( const std::string& name, bool save_data );

        // 查找插件
        KFPlugin* FindPlugin( const std::string& name );

        // 查找模块
        KFModule* FindModule( const std::string& name, const char* file, uint32 line );
        KFModule* FindModule( const std::string& name );

    protected:
        // 正在初始化的插件
        KFPlugin* _init_plugin = nullptr;

        // 插件列表
        std::vector<KFPlugin*> _plugins;

        // 命令列表
        KFQueue<KFDebugCommand> _commands;

        // 命令回调函数列表
        KFMapFunction<std::string, KFCmdFunction> _command_functions;

        // 注册的Run函数
        KFMapModuleFunction<uint32, KFRunFunction> _run_functions;
        KFMapModuleFunction<uint32, KFRunFunction> _after_run_functions;
    };
}

#endif