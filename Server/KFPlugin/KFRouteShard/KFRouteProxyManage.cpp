#include "KFRouteProxyManage.h"

namespace KFrame
{
    KFRouteProxyManage::KFRouteProxyManage()
    {

    }

    KFRouteProxyManage::~KFRouteProxyManage()
    {

    }

    KFRouteProxy* KFRouteProxyManage::AddRouteProxy( uint32 zoneid, uint64 serverid, uint64 handleid )
    {
        auto routezone = _kf_route_proxy.Create( serverid );
        routezone->_zone_id = zoneid;
        routezone->_server_id = serverid;
        routezone->_handle_id = handleid;

        return routezone;
    }

    void KFRouteProxyManage::RemoveRouteProxy( uint64 serverid )
    {
        _kf_route_proxy.Remove( serverid );
    }

    KFRouteProxy* KFRouteProxyManage::FindRouteProxy( uint64 serverid )
    {
        return _kf_route_proxy.Find( serverid );
    }

}