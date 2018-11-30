#ifndef __NET_SERVER_SERVICES_H__
#define __NET_SERVER_SERVICES_H__

#include "KFrame.h"
#include "KFNetServices.h"

namespace KFrame
{
    class KFNetServerServices : public KFNetServices
    {
    public:
        KFNetServerServices();
        virtual ~KFNetServerServices();

        // 初始化
        virtual void InitServices( uint32 eventcount, uint32 queuesize, uint32 messagetype );

        // 开启服务
        int32 StartServices( const KFNetSetting* kfsetting );

        // 关闭服务
        virtual void ShutServices();

    protected:
        // 处理新连接
        static void OnConnectionCallBack( uv_stream_t* pstream, int status );

        // 创建托管id
        uint32 MakeTrusteeID();

    protected:
        // 托管连接id
        uint32 _trustee_id;

        // uv tcp
        uv_tcp_t _uv_server;
    };
}

#endif