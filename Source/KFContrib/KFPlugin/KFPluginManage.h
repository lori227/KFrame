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

#define __REGISTER_MODULE__( name ) \
		_kf_plugin_manage->RegistModule< name##Plugin, name##Interface >( new name##Module() )

#define __UNREGISTER_MODULE__( name ) \
		_kf_plugin_manage->UnRegistModule< name##Plugin, name##Interface >()

#define __REGISTER_RUN_FUNCTION__( function ) \
		_kf_plugin_manage->RegisterRunFunction( _kf_plugin->_sort, this, function )

#define __UNREGISTER_RUN_FUNCTION__() \
		_kf_plugin_manage->UnRegisterRunFunction( _kf_plugin->_sort )

#define __REGISTER_AFTER_RUN_FUNCTION__( function ) \
		_kf_plugin_manage->RegisterAfterRunFunction( _kf_plugin->_sort, this, function )

#define __UNREGISTER_AFTER_RUN_FUNCTION__() \
		_kf_plugin_manage->UnRegisterAfterRunFunction( _kf_plugin->_sort )
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
			return dynamic_cast<T*>(RegistPlugin( typeid(T).name(), plugin ));
		}

		// 卸载插件
		template< class T >
		void UnRegistPlugin()
		{
			std::string name = typeid(T).name();
			UnRegistPlugin( name );
		}

		// 查找插件
		template< class T >
		T* FindPlugin()
		{
			std::string name = typeid(T).name();
			return dynamic_cast<T*>(FindPlugin( name ));
		}

		/////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////
		// 注册模块
		template< class PluginType, class ModuleType >
		void RegistModule( ModuleType* module )
		{
			auto plugin = FindPlugin< PluginType >();

			plugin->RegistModule< ModuleType >( module );
		}

		// 卸载模块
		template< class PluginType, class ModuleType >
		void UnRegistModule()
		{
			auto plugin = FindPlugin< PluginType >();

			plugin->UnRegistModule< ModuleType >();
		}

		// 查找模块
		template< class ModuleType >
		ModuleType* FindModule()
		{
			std::string name = typeid(ModuleType).name();
			return dynamic_cast<ModuleType*>(FindModule( name ));
		}

		/////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterCommandFunction( const std::string& command, T* object, void ( T::*handle )( const VectorString& ) )
		{
			auto kffunction = _command_functions.Create( command );
			kffunction->_function = std::bind( handle, object, std::placeholders::_1 );
		}

		void UnRegisterCommandFunction(const std::string& command)
		{
			_command_functions.Remove( command );
		}

		bool ProcessCommand( std::string& cmd, VectorString & param, std::string split = "," );

		/////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterRunFunction( uint32 sort, T* object, void ( T::*handle )() )
		{
			auto kffunction = _run_functions.Find( sort );
			if ( kffunction != nullptr )
			{
				return KFLogger::LogInit( KFLogger::Error, " sort[%u] run is already register!",
					sort );
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
				return KFLogger::LogInit( KFLogger::Error, " sort[%u] after run is already register!",
					sort );
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
		// 排序模块
		void SortPlugin();

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
static auto _kf_plugin_manage = KFrame::KFPluginManage::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif