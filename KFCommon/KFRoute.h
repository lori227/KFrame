#ifndef __KF_ROUTE_H__
#define __KF_ROUTE_H__

#include "KFDefine.h"

// 路由数据信息
namespace KFrame
{
    class Route
    {
    public:
        Route() {};
        Route( uint64 server_id, uint64 send_id, uint64 recv_id ) :
            _server_id( server_id ),
            _send_id( send_id ),
            _recv_id( recv_id )
        {

        }

    public:
        // 发送服务器id
        uint64 _server_id = 0u;

        // 发送者id
        uint64 _send_id = 0u;

        // 接受者id
        uint64 _recv_id = 0u;
    };


#define __ROUTE_SEND_ID__ route._send_id
#define __ROUTE_RECV_ID__ route._recv_id
#define __ROUTE_SERVER_ID__ route._server_id
}

#endif
