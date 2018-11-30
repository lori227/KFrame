#ifndef __KF_ROUTE_ZONE_MANAGE_H__
#define __KF_ROUTE_ZONE_MANAGE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRouteZone
    {
    public:
        KFRouteZone()
        {
            _zone_id = 0;
            _server_id = 0;
            _handle_id = 0;
        }

        uint32 _zone_id;
        uint64 _server_id;
        uint64 _handle_id;
    };

    // 匹配服务器管理器
    class KFRouteZoneManage : public KFSingleton< KFRouteZoneManage >
    {
    public:
        KFRouteZoneManage() = default;
        ~KFRouteZoneManage() = default;

        // 添加游戏区
        KFRouteZone* AddRouteZone( uint32 zoneid, uint64 serverid, uint64 handleid );

        // 删除游戏区
        void RemoveRouteZone( uint64 serverid );

        // 查找登录节点
        KFRouteZone* FindRouteZone( uint64 serverid );

    public:
        // 登录节点列表
        KFMap< uint64, uint64, KFRouteZone > _kf_route_zone;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_zone_manage = KFRouteZoneManage::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif