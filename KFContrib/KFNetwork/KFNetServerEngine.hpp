#ifndef __SERVER_ENGINE_H__
#define __SERVER_ENGINE_H__

#include "KFrame.h"
#include "KFNetEvent.hpp"
#include "KFNetHandle.hpp"
#include "KFNetServerService.hpp"

namespace KFrame
{
    class KFNetServerEngine
    {
    public:
        KFNetServerEngine();
        virtual ~KFNetServerEngine();

        // 初始化
        void InitEngine( uint32 max_queue_size, uint32 handle_count, uint32 message_type,
                         uint32 compress_type, uint32 compress_level, uint32 compress_length,
                         const std::string& encrypt_key, bool open_encrypt );

        // 初始化服务
        int32 StartEngine( const std::string& ip, uint32 port );

        // 逻辑
        void RunEngine( uint64 now_time );

        // 关闭引擎
        void ShutEngine();

        // 关闭连接
        bool CloseHandle( uint64 id, uint32 delay_time, const char* function, uint32 line );

        // 注册一个Handle
        std::shared_ptr<KFNetHandle> RegisterHandle( uint64 trustee_id, uint64 handle_id, uint64 object_id );

        // 获得一个Handle
        std::shared_ptr<KFNetHandle> FindNetHandle( uint64 handle_id );

        // 连接数量
        uint32 GetHandleCount();

        // 连接列表
        void GetHandleList( NetDataList& out_list );

        // 获得连接ip
        const std::string& GetHandleIp( uint64 handle_id );

        // 设置id
        bool BindObjectId( uint64 handle_id, uint64 object_id );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息
        void SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint64 exclude_id = 0 );
        bool SendNetMessage( uint64 handle_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );
        bool SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );

        void SendMessageToName( const std::string& name, uint32 msg_id, const char* data, uint32 length );
        void SendMessageToType( const std::string& type, uint32 msg_id, const char* data, uint32 length );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 绑定消息事件
        template<class T>
        void BindNetFunction( T* object, void ( T::*handle )( const Route& route, uint32 msg_id, const char* data, uint32 length ) )
        {
            _net_server_service->_net_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
        }

        // 绑定断开事件
        template<class T>
        void BindEventFunction( uint32 type, T* object, void ( T::*handle )( const KFNetData* ) )
        {
            KFNetEventFunction function = std::bind( handle, object, std::placeholders::_1 );
            auto event_function = _event_function_list.Create( type );
            event_function->SetFunction( function );
        }

    protected:
        // 连接时间回调
        void OnServerConnected( std::shared_ptr<KFNetEventData>& event_data );

        // 连接关闭回调
        void OnServerShutDown( std::shared_ptr<KFNetEventData>& event_data );

        // 连接断开时间
        void OnServerDisconnect( std::shared_ptr<KFNetEventData>& event_data );

    protected:
        // 删除托管连接
        void RunRemoveTrusteeHandle();

        // 判断托管超时
        void RunCheckBindTimeout();

        // 需要关闭的连接
        void RunCloseHandle();

        // 处理网络消息
        void RunTrusteeMessage( uint64 now_time );
        void RunHandleMessage( uint64 now_time );

    protected:
        // 服务器服务
        KFNetServerService* _net_server_service;

        // 等待验证的托管连接
        KFHashMap<uint64, KFNetHandle> _trustee_handle_list;

        // 有效连接列表
        KFHashMap<uint64, KFNetHandle> _work_handle_list;

        // 需要验证超时的连接
        UInt64Map _bind_timeout_list;

        // 已经注册的链接
        UInt64Set _remove_trustee_list;

        // 需要关闭的连接
        UInt64Map _close_handle_list;
    private:
        // 每一帧处理消息个数
        uint32 _handle_message_count = 10;

        // 网络事件
        KFMapFunction<uint32, KFNetEventFunction> _event_function_list;
    };
}

#endif