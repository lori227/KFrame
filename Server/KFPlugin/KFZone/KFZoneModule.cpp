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

        kfglobal->_zone_id = kfzone->_id;
        kfglobal->_app_id = KFUtility::CalcZoneServerId( kfglobal->_app_id, kfzone->_id );

        // 修改服务器ip
        _kf_ip_address->SetZoneIpAddress( kfzone->_id, kfzone->_ip );
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

    bool KFZoneModule::IsServerSameZone( uint32 serverid )
    {
        auto zoneid = KFUtility::CalcServerZoneId( serverid );
        return zoneid == _kf_zone_config->_kf_zone._id;
    }

    bool KFZoneModule::IsPlayerSameZone( uint32 playerid )
    {
        auto zoneid = KFUtility::CalcZoneId( playerid );
        return zoneid == _kf_zone_config->_kf_zone._id;
    }

}
