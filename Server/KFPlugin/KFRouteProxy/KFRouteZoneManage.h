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
        uint32 _server_id;
        uint32 _handle_id;
    };

    // 匹配服务器管理器
    class KFRouteZoneManage : public KFSingleton< KFRouteZoneManage >
    {
    public:
        KFRouteZoneManage() = default;
        ~KFRouteZoneManage() = default;

        // 添加游戏区
        KFRouteZone* AddRouteZone( uint32 zoneid, uint32 serverid, uint32 handleid );

        // 删除游戏区
        void RemoveRouteZone( uint32 serverid );

        // 查找登录节点
        KFRouteZone* FindRouteZone( uint32 serverid );

    public:
        // 登录节点列表
        KFMap< uint32, uint32, KFRouteZone > _kf_route_zone;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_zone_manage = KFRouteZoneManage::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif