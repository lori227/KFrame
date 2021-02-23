#ifndef __KF_ROUTE_SERVICE_H__
#define __KF_ROUTE_SERVICE_H__
#include "KFrame.h"

namespace KFrame
{
    // 转发服务
    class KFRouteService
    {
    public:
        KFRouteService() = default;
        ~KFRouteService() = default;

        // 添加服务器
        void AddServer( uint64 server_id );

        // 添加对象
        void AddObject( uint64 server_id, uint64 object_id, uint32 object_count );

        // 删除服务器
        void RemoveServer( uint64 server_id );

        // 删除对象
        void RemoveObject( uint64 server_id, uint64 object_id, uint32 object_count );

        // 获得随机服务器
        uint64 RandServer( uint64 client_id );

        // 获得最小负载服务器
        uint64 BalanceServer();

        // 获得对象服务器
        uint64 ObjectServer( uint64 object_id );

    protected:
        // 计算最小负载
        void CalcMinBalance();

    public:
        // 服务类型/名字
        std::string _name;

        // 服务器hash列表
        KFHash _server_hash;

        // 对象列表( 对象所在的服务器 )
        std::unordered_map<uint64, UInt64Set> _object_list;

        // 服务器的对象总数
        UInt64Map _server_object_count_list;

        // 最小负载服务器id
        uint32 _min_balance_count = 0u;
        uint64 _min_balance_server_id = 0u;
    };
}


#endif