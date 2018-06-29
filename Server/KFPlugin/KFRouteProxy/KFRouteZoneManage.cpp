#include "KFRouteZoneManage.h"

namespace KFrame
{
    KFRouteZoneManage::KFRouteZoneManage()
    {

    }

    KFRouteZoneManage::~KFRouteZoneManage()
    {

    }

    KFRouteZone* KFRouteZoneManage::AddRouteZone( uint32 zoneid, uint32 serverid, uint32 handleid )
    {
        auto routezone = _kf_route_zone.Create( serverid );
        routezone->_zone_id = zoneid;
        routezone->_server_id = serverid;
        routezone->_handle_id = handleid;

        return routezone;
    }

    void KFRouteZoneManage::RemoveRouteZone(  uint32 serverid )
    {
        _kf_route_zone.Remove( serverid );
    }

    KFRouteZone* KFRouteZoneManage::FindRouteZone( uint32 serverid )
    {
        return _kf_route_zone.Find( serverid );
    }

}