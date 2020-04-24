#ifndef __SERVER_ENGINE_H__
#define __SERVER_ENGINE_H__

#include "KFrame.h"
#include "KFNetHandle.h"
#include "KFNetEvent.h"
#include "KFNetServerServices.h"

namespace KFrame
{
    class KFNetServerEngine
    {
    public:
        KFNetServerEngine();
        virtual ~KFNetServerEngine();

        // 初始化
        void InitEngine( uint32 maxqueuesize, uint32 messagetype, uint32 compress );

        // 初始化服务
        int32 StartEngine( const std::string& ip, uint32 port );

        // 逻辑
        void RunEngine( uint64 nowtime );

        // 关闭引擎
        void ShutEngine();

        // 关闭连接
        bool CloseHandle( uint64 id, uint32 delaytime, const char* function, uint32 line );

        // 注册一个Handle
        KFNetHandle* RegisteHandle( uint64 trusteeid, uint64 handleid, uint64 objectid );

        // 获得一个Handle
        KFNetHandle* FindNetHandle( uint64 handleid );

        // 连接数量
        uint32 GetHandleCount();

        // 连接列表
        void GetHandleList( NetDataList& outlist );

        // 获得连接ip
        const std::string& GetHandleIp( uint64 handleid );

        // 设置id
        bool BindObjectId( uint64 handleid, uint64 objectid );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息
        void SendNetMessage( uint32 msgid, const char* data, uint32 length, uint64 excludeid = 0 );
        bool SendNetMessage( uint64 handleid, uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );
        bool SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );

        void SendMessageToName( const std::string& name, uint32 msgid, const char* data, uint32 length );
        void SendMessageToType( const std::string& type, uint32 msgid, const char* data, uint32 length );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 绑定消息事件
        template< class T >
        void BindNetFunction( T* object, void ( T::*handle )( const Route& route, uint32 msgid, const char* data, uint32 length ) )
        {
            _net_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
        }

        // 绑定断开事件
        template< class T >
        void BindLostFunction( T* object, void ( T::*handle )( const KFNetData* ) )
        {
            _server_lost_function = std::bind( handle, object, std::placeholders::_1 );
        }

    protected:
        // 连接时间回调
        void OnServerConnected( const KFNetEventData* eventdata );

        // 连接关闭回调
        void OnServerShutDown( const KFNetEventData* eventdata );

        // 连接断开时间
        void OnServerDisconnect( const KFNetEventData* eventdata );

    protected:
        // 删除托管连接
        void RunRemoveTrusteeHandle();

        // 判断托管超时
        void RunCheckTrusteeTimeout();

        // 需要关闭的连接
        void RunCloseHandle();

        // 处理网络消息
        void RunTrusteeMessage( uint64 nowtime );
        void RunHandleMessage( uint64 nowtime );

    public:
        // 有效连接列表
        KFHashMap< uint64, uint64, KFNetHandle > _kf_handles;

    protected:
        // 服务器服务
        KFNetServerServices* _net_server_services;

        // 等待验证的托管连接
        KFHashMap< uint64, uint64, KFNetHandle > _trustee_handles;

        // 已经注册的链接
        UInt64Set _remove_trustees;

        // 需要关闭的连接
        UInt64Map _close_handles;
    private:

        // 消息函数
        KFMessageFunction _net_function;

        // 断开连接事件
        KFNetEventFunction _server_lost_function;
    };
}

#endif