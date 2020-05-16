#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

#include "KFrame.h"
#include "KFNetConnector.h"

struct uv_tcp_s;
typedef struct uv_tcp_s uv_tcp_t;

struct uv_connect_s;
typedef struct uv_connect_s uv_connect_t;

struct uv_timer_s;
typedef struct uv_timer_s uv_timer_t;

namespace KFrame
{
    class KFNetClientServices;
    class KFNetClient : public KFNetConnector
    {
    public:
        KFNetClient();
        virtual ~KFNetClient();

        ////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////
        // 初始化服务
        void StartClient( KFNetClientServices* netservices, const KFNetData& netdata );

        // 关闭服务
        void CloseClient();

        // 开始连接
        virtual void StartSession();

        // 关闭逻辑
        virtual void CloseSession();

        // 断开事件
        virtual void OnDisconnect( int32 code, const char* function, uint32 line );

        // 设置压缩数据
        void InitCompressEncrypt( uint32 compresstype, uint32 compresslevel, uint32 compresslength, const std::string& encryptkey, bool openencrypt );

    private:
        // 启动连接定时器
        void StartConnectTimer( uint32 time );

        // 连接定时器回调
        static void OnTimerConnectCallBack( uv_timer_t* handle );

        // 尝试连接
        void TryConnect();
        static void OnConnectCallBack( uv_connect_t* handle, int status );

        // 连接失败
        void ConnectFailed( int32 status );

        // 连接成功
        void ConnectSuccess( uv_stream_t* );

        // 关闭回调
        static void OnShutDownCallBack( uv_handle_t* handle );

    public:
        // 网络配置
        KFNetData _net_data;

    private:
        // 客户端服务
        uv_tcp_t* _uv_client;

        // 连接句柄
        uv_connect_t* _uv_connect;

        // 连接定时器
        uv_timer_t* _uv_connect_timer;

        // 压缩加密数据
        KFNetCompressEncrypt _net_compress_encrypt;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif