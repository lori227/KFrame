#include "KFRouteZoneManage.h"

namespace KFrame
{
    KFRouteZone* KFRouteZoneManage::AddRouteZone( uint32 zoneid, uint64 serverid, uint64 handleid )
    {
        auto routezone = _kf_route_zone.Create( serverid );
        routezone->_zone_id = zoneid;
        routezone->_server_id = serverid;
        routezone->_handle_id = handleid;

        return routezone;
    }

    void KFRouteZoneManage::RemoveRouteZone( uint64 serverid )
    {
        _kf_route_zone.Remove( serverid );
    }

    KFRouteZone* KFRouteZoneManage::FindRouteZone( uint64 serverid )
    {
        return _kf_route_zone.Find( serverid );
    }

}