#include "KFZoneModule.h"
#include "KFZoneConfig.h"

namespace KFrame
{
    void KFZoneModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_zone_config, false );
    }

    void KFZoneModule::AfterLoad()
    {
        auto* kfzone = &_kf_zone_config->_zone;
        if ( kfzone->_id == _invalid_int )
        {
            kfzone->_id = KFGlobal::Instance()->_zone_id;

            // 逻辑id
            if ( kfzone->_logic_id == _invalid_int )
            {
                kfzone->_logic_id = kfzone->_id;
            }

            // 名字
            kfzone->_name = __FORMAT__( kfzone->_name, kfzone->_id );
        }

        // 修改服务器ip
        if ( !kfzone->_ip.empty() )
        {
            _kf_ip_address->SetZoneIpAddress( kfzone->_ip );
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
        return &_kf_zone_config->_zone;
    }

    bool KFZoneModule::IsServerSameZone( uint32 serverid )
    {
        auto zoneid = KFUtility::CalcServerZoneId( serverid );
        return zoneid == _kf_zone_config->_zone._id;
    }

    bool KFZoneModule::IsPlayerSameZone( uint32 playerid )
    {
        auto zoneid = KFUtility::CalcZoneId( playerid );
        return zoneid == _kf_zone_config->_zone._id;
    }

}
