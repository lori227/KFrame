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
        KFAppID kfappid( serverid );
        return kfappid._union._app_data._zone_id == KFGlobal::Instance()->_app_id._union._app_data._zone_id;
    }

    bool KFZoneModule::IsPlayerSameZone( uint64 playerid )
    {
        auto zoneid = KFUtility::CalcZoneId( playerid );
        return zoneid == KFGlobal::Instance()->_app_id._union._app_data._zone_id;
    }

}
