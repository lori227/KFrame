#include "KFConnectionModule.h"
#include "KFConnectionConfig.h"

namespace KFrame
{
	KFConnectionModule::KFConnectionModule()
	{
	}

	KFConnectionModule::~KFConnectionModule()
	{

	}

	void KFConnectionModule::InitModule()
	{
		_kf_config->AddConfig( _kf_connection_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}
	

	void KFConnectionModule::ShutDown()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	uint32 KFConnectionModule::GetListenPort() const
	{
		return _kf_connection_config->_listen_port;
	}

	const std::string& KFConnectionModule::FindPlatformAddress( uint32 id )
	{
		return _kf_connection_config->FindPlatformAddress( id );
	}

	const KFConnection* KFConnectionModule::FindMasterConnection( const std::string& name )
	{
		return _kf_connection_config->FindMasterConnection( name );
	}

	void KFConnectionModule::SetMasterConnection( const std::string& name, uint32 zoneid )
	{
		_kf_connection_config->SetMasterConnection( name, zoneid );
	}
}
