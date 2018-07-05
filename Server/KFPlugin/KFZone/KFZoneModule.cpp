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
        __KF_ADD_CONFIG__( _kf_zone_config, true );
    }

    void KFZoneModule::AfterLoad()
    {
        auto kfglobal = KFGlobal::Instance();
        auto kfzone = &_kf_zone_config->_kf_zone;

        auto id = kfglobal->_app_id % 1000;
        kfglobal->_app_id = id + ( kfzone->_id + 1000 ) * 1000;

        // 修改服务器ip
        _kf_ip_address->SetZoneIpAddress( kfzone->_id, kfglobal->_app_id, kfzone->_ip );
    }

    void KFZoneModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
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
