#ifndef __NET_SESSION_H__
#define __NET_SESSION_H__

#include "KFrame.h"
#include "KFQueue.h"
#include "KFNetDefine.h"
#include "KFNetMessage.h"

struct uv_stream_s;
typedef struct uv_stream_s uv_stream_t;

struct uv_handle_s;
typedef struct uv_handle_s uv_handle_t;

struct uv_write_s;
typedef uv_write_s uv_write_t;

struct uv_buf_t;

namespace KFrame
{
    class KFNetSession
    {
    public:
        KFNetSession();
        ~KFNetSession();

        /////////////////////////////////////////////////////////////////////////
        // 处理连接成功
        virtual void OnConnect( uv_stream_t* uvstream );

        // 断线
        virtual void OnDisconnect( int32 code, const char* function, uint32 line );

        // 开始连接
        virtual void StartSession() {}

        // 关闭连接
        virtual void CloseSession();
        /////////////////////////////////////////////////////////////////////////

        // 是否已经连接
        bool IsConnected() const;

        // 是否需要发送
        bool IsNeedSend();

        // 开始发送消息
        void StartSendMessage();

    protected:
        // 初始化
        void InitSession( uint64 id, uint32 queuecount, uint32 headlength );

        // 添加发送消息
        bool AddSendMessage( KFNetMessage* message );

        // 添加收到的消息
        bool AddRecvMessage( KFNetMessage* message );
    protected:
        // 开始接受消息
        void StartRecvData();

        // 接收数据
        void OnRecvData( const char* buffer, uint32 length );

        // 申请接受内存
        static void AllocRecvBuffer( uv_handle_t* handle, size_t size, uv_buf_t* pbuffer );

        // 接受消息
        static void OnRecvCallBack( uv_stream_t* pstream, int64 length, const uv_buf_t* pbuffer );

        // 格式化消息
        void ParseBuffToMessage();

        // 判断消息的有效性
        KFNetHead* CheckReciveBuffValid( uint32 position );

    protected:

        // 发送消息
        void SendBuffer( char* buffer, uint32 length );
        void OnSendOK();

        // 发送消息回调
        static void OnSendCallBack( uv_write_t* uvwrite, int32 status );

        // 准备发送数据
        uint32 SendQueueMessage();

        // 检测发送消息长度
        bool CheckBufferLength( uint32 totallength, uint32 messagelength );

        // 是否是服务器
        bool IsServerSession();

    public:
        // 连接id
        uint64 _session_id = 0u;

        // 绑定对象id
        uint64 _object_id = 0u;

        // 是否已经连接
        std::atomic<bool> _is_connected;

        // 已经关闭
        std::atomic<bool> _is_shutdown;

    protected:
        // 发送消息队列
        KFQueue< KFNetMessage > _send_queue;

        // 发送队列满
        bool _is_send_queue_full = false;

        // 收消息队列
        KFQueue< KFNetMessage > _recv_queue;

        // 接受队列满
        bool _is_recv_queue_full = false;

    private:
        // 发送消息句柄
        uv_write_t* _uv_write = nullptr;
        uv_stream_t* _uv_stream = nullptr;

    private:
        // 消息头长度
        uint32 _message_head_length = 0u;

        // 接收消息buffer
        uint32 _receive_length = 0u;
        char _receive_buff[ KFNetDefine::MaxRecvBuffLength ];

        // 请求发送的buffer
        uint32 _send_length = 0u;
        std::atomic<bool> _is_sending;
        char _req_send_buffer[ KFNetDefine::MaxReqBuffLength ];

        // 请求接受的buffer
        char _req_recv_buffer[ KFNetDefine::MaxReqBuffLength ];
    };

}

#endif