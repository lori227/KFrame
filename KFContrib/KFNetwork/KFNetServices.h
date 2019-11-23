#ifndef __NET_SERVICES_H__
#define __NET_SERVICES_H__

#include "KFNetEvent.h"

struct uv_loop_s;
typedef struct uv_loop_s uv_loop_t;

struct uv_async_s;
typedef struct uv_async_s uv_async_t;

namespace KFrame
{
    class KFNetSession;
    class KFNetServices
    {
    public:
        KFNetServices();
        virtual ~KFNetServices();

        // 初始化
        virtual void InitServices( uint32 eventcount, uint32 queuecount, uint32 messagetype, uint32 compress );

        // 开始服务
        virtual int32 StartServices( const KFNetData* netdata );

        // 关闭服务
        virtual void ShutServices();

        // 获得buffer
        char* GetBuffAddress( uint32 msgid, uint32 length );

        // 发送事件到网络服务
        void SendEventToServices( KFNetSession* netsession, uint32 evnettype );

        // 消息加密
        const char* Encode( const char* data, uint32& length );

        // 消息解密
        const char* Decode( const char* data, uint32& length );

    protected:
        // 线程逻辑
        static void RunServices( void* argument );

        // 异步事件回调
        static void OnAsyncEventCallBack( uv_async_t* handle );

        // 关闭服务
        static void OnAsyncCloseCallBack( uv_async_t* handle );

        // 计算压缩buff长度
        void CalcCompressLength( uint32 length );
    public:
        // 消息序列化buff地址
        char* _buff_address = nullptr;
        uint32 _buff_length = 0u;

        // 解压buff
        char* _compress_address = nullptr;
        uint32 _compress_length = 0u;

        // 网络事件
        KFNetEvent* _net_event = nullptr;

        // uv 服务
        uv_loop_t* _uv_loop = nullptr;

        // 消息队列大小
        uint32 _queue_size = 1000u;

        // 是否已经关闭
        std::atomic<bool> _is_shutdown;

        // 当前时间
        uint64 _now_time = 0u;

        // 服务器消息类型
        uint32 _message_type = 0u;
    protected:
        // 线程逻辑标识
        std::atomic<bool> _thread_run;

        // 线程id
        int64 _thread_id = 0u;

        // 压缩等级
        uint32 _compress = 0u;

        // 异步事件
        uv_async_t* _uv_event_async = nullptr;
        void* _uv_event_mutex = nullptr;
        std::map< KFNetSession*, uint32 > _event_session;

        // 关闭服务
        uv_async_t* _uv_close_async = nullptr;
    };
}

#endif