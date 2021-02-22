#ifndef __NET_SERVER_SERVICE_H__
#define __NET_SERVER_SERVICE_H__

#include "KFrame.h"
#include "KFNetService.hpp"

struct uv_tcp_s;
typedef struct uv_tcp_s uv_tcp_t;

namespace KFrame
{
    class KFNetServerService : public KFNetService
    {
    public:
        KFNetServerService();
        virtual ~KFNetServerService();

        // 初始化
        virtual void InitService( uint32 event_count, uint32 queue_size, uint32 message_type );

        // 开启服务
        int32 StartService( const KFNetData* net_data );

        // 关闭服务
        virtual void CloseService();

    protected:
        // 处理新连接
        static void OnConnectionCallBack( uv_stream_t* stream, int status );

        // 创建托管id
        uint64 MakeTrusteeID();

    protected:
        // 托管连接id
        uint64 _trustee_id = 0u;

        // uv tcp
        uv_tcp_t* _uv_server;
    };
}

#endif