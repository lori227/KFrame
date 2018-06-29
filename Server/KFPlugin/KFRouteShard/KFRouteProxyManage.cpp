#include "KFRouteProxyManage.h"

namespace KFrame
{
    KFRouteProxyManage::KFRouteProxyManage()
    {

    }

    KFRouteProxyManage::~KFRouteProxyManage()
    {

    }

    KFRouteProxy* KFRouteProxyManage::AddRouteProxy( uint32 zoneid, uint32 serverid, uint32 handleid )
    {
        auto routezone = _kf_route_proxy.Create( serverid );
        routezone->_zone_id = zoneid;
        routezone->_server_id = serverid;
        routezone->_handle_id = handleid;

        return routezone;
    }

    void KFRouteProxyManage::RemoveRouteProxy( uint32 serverid )
    {
        _kf_route_proxy.Remove( serverid );
    }

    KFRouteProxy* KFRouteProxyManage::FindRouteProxy( uint32 serverid )
    {
        return _kf_route_proxy.Find( serverid );
    }

}