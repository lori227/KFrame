#ifndef __KF_ROUTER_H__
#define __KF_ROUTER_H__

#include "KFInclude.h"

// 路由数据信息
namespace KFrame
{
    class Route
    {
    public:
        Route() {};
        Route( uint64 serverid, uint64 sendid, uint64 recvid ) :
            _server_id( serverid ),
            _send_id( sendid ),
            _recv_id( recvid )
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
