#include "KFOptionModule.h"
#include "KFOptionConfig.h"

namespace KFrame
{
	KFOptionModule::KFOptionModule()
	{
	}

	KFOptionModule::~KFOptionModule()
	{
	}

	void KFOptionModule::InitModule()
	{
		_kf_config->AddConfig( _kf_option_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}

	void KFOptionModule::BeforeRun()
	{
	}

	void KFOptionModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
	}
	
	////////////////////////////////////////////////////////////////
	const std::string& KFOptionModule::GetString( const std::string& name, uint32 key /* = 0 */ )
	{
		return _kf_option_config->FindOption( name, key );
	}
	
	uint32 KFOptionModule::GetUInt32( const std::string& name, uint32 key /* = 0 */ )
	{
		auto& option = _kf_option_config->FindOption( name, key );
		return KFUtility::ToValue< uint32 >( option );
	}
}