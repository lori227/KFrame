#include "KFZoneModule.h"
#include "KFZoneConfig.h"

namespace KFrame
{
    KFZoneModule::KFZoneModule()
    {
        _kf_zone = nullptr;
    }

    KFZoneModule::~KFZoneModule()
    {

    }

    void KFZoneModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_zone_config, false );
    }

    void KFZoneModule::AfterLoad()
    {
        auto kfglobal = KFGlobal::Instance();
        _kf_zone = _kf_zone_config->FindZone( kfglobal->_app_channel, kfglobal->_zone_id );
        if ( _kf_zone->_id == 0 )
        {
            _kf_zone->_id = kfglobal->_zone_id;
            _kf_zone->_channel = kfglobal->_app_channel;

            // 逻辑id
            if ( _kf_zone->_logic_id == 0 )
            {
                _kf_zone->_logic_id = _kf_zone->_id;
            }

            // 名字
            _kf_zone->_name = __FORMAT__( _kf_zone->_name, _kf_zone->_id );
        }

        // 修改服务器ip
        if ( !_kf_zone->_ip.empty() )
        {
            _kf_ip_address->SetZoneIpAddress( _kf_zone->_ip );
        }
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
        return _kf_zone;
    }

    bool KFZoneModule::IsServerSameZone( uint32 serverid )
    {
        auto zoneid = KFUtility::CalcServerZoneId( serverid );
        return zoneid == _kf_zone->_id;
    }

    bool KFZoneModule::IsPlayerSameZone( uint32 playerid )
    {
        auto zoneid = KFUtility::CalcZoneId( playerid );
        return zoneid == _kf_zone->_id;
    }

}
