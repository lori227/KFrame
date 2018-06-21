#include "KFZoneModule.h"
#include "KFZoneConfig.h"

namespace KFrame
{
	KFZoneModule::KFZoneModule()
	{
	}

	KFZoneModule::~KFZoneModule()
	{

	}

	void KFZoneModule::InitModule()
	{
		_kf_config->AddConfig( _kf_zone_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}
	
	void KFZoneModule::AfterLoad()
	{
		auto id = ( _kf_zone_config->_kf_zone._id + 1000 ) * 1000;
		KFGlobal::Instance()->_app_id += id;
	
		// 修改id
		_kf_connection->SetMasterConnection( KFField::_zone, _kf_zone_config->_kf_zone._id );
	}

	void KFZoneModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const KFZone* KFZoneModule::GetZone() const
	{
		return &_kf_zone_config->_kf_zone;
	}

	uint32 KFZoneModule::GetServerZoneId( uint32 serverid )
	{
		return ( serverid / 1000 ) % 1000;
	}

	uint32 KFZoneModule::GetPlayerZoneId( uint32 playerid )
	{
		return ( playerid / KFZoneEnum::MaxPlayerCount );
	}

	bool KFZoneModule::IsServerSameZone( uint32 serverid )
	{
		auto zoneid = GetServerZoneId( serverid );
		return zoneid == _kf_zone_config->_kf_zone._id;
	}

	bool KFZoneModule::IsPlayerSameZone( uint32 playerid )
	{
		auto zoneid = GetPlayerZoneId( playerid );
		return zoneid == _kf_zone_config->_kf_zone._id;
	}

}
