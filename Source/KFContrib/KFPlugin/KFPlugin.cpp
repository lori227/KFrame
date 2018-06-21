#include "KFPlugin.h"
#include "KFTime/KFClock.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
	KFPlugin::KFPlugin()
	{
		_sort = 0;
	}

	KFPlugin::~KFPlugin()
	{
	}

	void KFPlugin::InitModule()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->InitModule();
		}
	}

	void KFPlugin::LoadConfig()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->LoadConfig();
		}
	}

	void KFPlugin::AfterLoad()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->AfterLoad();
		}
	}

	void KFPlugin::BeforeRun()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->BeforeRun();
		}
	}
	
	void KFPlugin::BeforeShut()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->BeforeShut();
		}
	}

	void KFPlugin::ShutDown()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->ShutDown();
		}
	}

	void KFPlugin::AfterShut()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->AfterShut();
		}
	}

	void KFPlugin::OnceRun()
	{
		for ( auto& iter : _kf_module )
		{
			iter.second->OnceRun();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	void KFPlugin::AddModule( const std::string& name, KFModule* module )
	{
		RemoveModule( name );

		_kf_module.insert( std::make_pair( name, module ) );
	}

	void KFPlugin::RemoveModule( const std::string& name )
	{
		auto iter = _kf_module.find( name );
		if ( iter == _kf_module.end() )
		{
			return;
		}

		delete iter->second;
		_kf_module.erase( iter );
	}
	
	KFModule* KFPlugin::FindModule( const std::string& name )
	{
		auto iter = _kf_module.find( name );
		if ( iter == _kf_module.end() )
		{
			return nullptr;
		}

		return iter->second;
	}

	void KFPlugin::RegistModule( const std::string& name, KFModule* module )
	{
		module->_class_name = name;
		module->_plugin_name = _class_name;
		module->_kf_plugin_manage = _kf_plugin_manage;
		module->Initialize( this );

		AddModule( name, module );
	}

	void KFPlugin::UnRegistModule( const std::string& name )
	{
		auto module = FindModule( name );
		if ( module == nullptr )
		{
			return;
		}

		module->BeforeShut();
		module->ShutDown();
		module->AfterShut();
		RemoveModule( name );
	}


}
