#ifndef __NET_SERVICE_H__
#define __NET_SERVICE_H__

#include "KFNetEvent.hpp"

struct uv_loop_s;
typedef struct uv_loop_s uv_loop_t;

struct uv_async_s;
typedef struct uv_async_s uv_async_t;

namespace KFrame
{
    class KFNetService
    {
    public:
        KFNetService();
        virtual ~KFNetService();

        // 初始化
        virtual void InitService( uint32 event_count, uint32 queue_count, uint32 message_type );

        // 开始服务
        virtual int32 StartService( const KFNetData* net_data );

        // 关闭服务
        virtual void CloseService();

        // 获得buffer
        char* GetBuffAddress( uint32 msg_id, uint32 msg_length );

        // 发送事件到网络服务
        void SendEventToService( uint32 event_type, std::shared_ptr<KFNetSession> net_session );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化压缩设置
        void InitCompress( uint32 compress_type, uint32 compress_level, uint32 compress_length );

        // 初始化加密设置
        void InitEncrypt( const std::string& encrypt_key, bool open_encrypt );

        // 消息加密
        std::tuple< const char*, uint32, uint16 > Encode( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32 length );

        // 消息解密
        std::tuple< const char*, uint32 > Decode( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32 length, uint16 flag );

    protected:
        // 线程逻辑
        static void RunServices( void* argument );

        // 异步事件回调
        static void OnAsyncEventCallBack( uv_async_t* handle );

        // 关闭服务
        static void OnAsyncCloseCallBack( uv_async_t* handle );

        // 压缩数据
        const char* CompressData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length );
        const char* DeCompressData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length );

        // 计算压缩buff长度
        void LZ4CalcCompressLength( const char* data, uint32 length, bool is_compress );
        void ZSTDCalcCompressLength( const char* data, uint32 length, bool is_compress );

        // 加密数据
        const char* EncryptData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length );
        const char* DeEncryptData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length );

    public:
        // 消息序列化buff地址
        char* _serialize_buff = nullptr;
        uint32 _max_serialize_buff_length = 0u;

        // 解压buff
        char* _compress_buff = nullptr;
        uint32 _max_compress_buff_length = 0u;

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

        // 压缩和加密设置
        KFNetCompressEncrypt _compress_encrypt;

        // 消息函数
        KFMessageFunction _net_function = nullptr;
    protected:
        // 线程逻辑标识
        std::atomic<bool> _thread_run;

        // 线程id
        int64 _thread_id = 0u;

        // 异步事件
        uv_async_t* _uv_event_async = nullptr;
        KFQueue<KFAsyncEvent> _async_event_list;

        // 关闭服务
        uv_async_t* _uv_close_async = nullptr;
    };
}

#endif