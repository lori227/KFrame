#include "KFZoneModule.hpp"
#include "KFZoneConfig.hpp"

namespace KFrame
{
    void KFZoneModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_zone_config, false );
    }

    void KFZoneModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_zone_config );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFZone* KFZoneModule::GetZone()
    {
        return &_kf_zone_config->_zone;
    }

    const KFZone* KFZoneModule::GetZone( uint32 zoneid )
    {
        return _kf_zone_config->FindZone( zoneid );
    }

    bool KFZoneModule::IsServerSameZone( uint64 serverid )
    {
        static KFAppId _kf_app_id;
        _kf_app_id.FromUInt64( serverid );

        return _kf_app_id.GetZoneId() == KFGlobal::Instance()->_app_id->GetZoneId();
    }

    bool KFZoneModule::IsPlayerSameZone( uint64 playerid )
    {
        auto zoneid = KFUtility::CalcZoneId( playerid );
        return zoneid == KFGlobal::Instance()->_app_id->GetZoneId();
    }

}
