#ifndef __NET_SERVICES_H__
#define __NET_SERVICES_H__

#include "KFNetSetting.h"
#include "KFNetEvent.h"

namespace KFrame
{
    class KFNetSession;
    class KFNetServices
    {
    public:
        KFNetServices();
        virtual ~KFNetServices();

        // 初始化
        virtual void InitServices( uint32 eventcount, uint32 queuecount );

        // 开始服务
        virtual int32 StartServices( const KFNetSetting* kfsetting );

        // 关闭服务
        virtual void ShutServices();

        // 获得buffer
        char* GetBuffAddress( uint32 msgid, uint32 length );

        // 发送消息
        void SendNetMessage( KFNetSession* netsession );

        // 开始连接
        void StartSession( KFNetSession* netsession );

        // 关闭连接
        void CloseSession( KFNetSession* netsession );

    protected:
        // 线程逻辑
        static void RunServices( void* argument );

        // 异步关闭回调
        static void OnAsyncShutCallBack( uv_async_t* handle );

        // 异步发送回调
        static void OnAsyncSendCallBack( uv_async_t* handle );

        // 异步连接回调
        static void OnAsyncConnectCallBack( uv_async_t* handle );
    public:
        // 消息序列化buff地址
        char* _buff_address;
        uint32 _buff_length;

        // 网络事件
        KFNetEvent* _net_event;

        // uv 服务
        uv_loop_t* _uv_loop;

        // 消息队列大小
        uint32 _queue_size;

    protected:
        // 线程逻辑标识
        volatile bool _thread_run;

        // 线程id
        uv_thread_t _thread_id;

        // 异步发消息
        uv_async_t _uv_send_async;
        uv_mutex_t _uv_send_mutex;
        std::set< KFNetSession* > _send_session;

        // 异步关闭
        uv_async_t _uv_shut_async;
        uv_mutex_t _uv_shut_mutex;
        std::set< KFNetSession* > _shut_session;

        // 异步连接( 客户端使用 )
        uv_async_t _uv_connect_async;
        uv_mutex_t _uv_connect_mutex;
        std::set< KFNetSession* > _connect_session;
    };
}

#endif