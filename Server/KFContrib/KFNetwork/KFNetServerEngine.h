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
        void InitEngine( uint32 maxqueuesize );

        // 初始化服务
        int32 StartEngine( const std::string& ip, uint32 port, uint32 maxconnect = 20000 );

        // 逻辑
        void RunEngine( uint64 nowtime );

        // 关闭引擎
        void ShutEngine();

        // 关闭连接
        bool CloseHandle( uint32 id, uint32 delaytime, const char* function, uint32 line );

        // 注册一个Handle
        KFNetHandle* RegisteHandle( uint32 trusteeid, uint32 handleid );

        // 获得一个Handle
        KFNetHandle* FindNetHandle( uint32 handleid );

        // 连接数量
        uint32 GetHandleCount();

        // 连接列表
        void GetHandleList( std::list< uint32 >& handlelist );

        // 获得连接ip
        const std::string& GetHandleIp( uint32 handleid );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息
        void SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid = 0 );
        void SendNetMessage( const std::string& name, uint32 msgid, const char* data, uint32 length );

        bool SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length );
        bool SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 绑定消息事件
        template< class T >
        void BindNetFunction( T* object, void ( T::*handle )( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length ) )
        {
            _net_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
        }

        // 绑定断开事件
        template< class T >
        void BindLostFunction( T* object, void ( T::*handle )( uint32 id, const std::string& name, const std::string& type ) )
        {
            _server_lost_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
        }

    protected:
        // 连接时间回调
        void OnServerConnected( const KFEventData* eventdata );

        // 连接关闭回调
        void OnServerShutDown( const KFEventData* eventdata );

        // 连接断开时间
        void OnServerDisconnect( const KFEventData* eventdata );

    protected:

        // 执行删除托管连接
        void RunRemoveTrusteeHandle();

        // 判断托管超时
        void RunCheckTrusteeTimeout();

        // 需要关闭的连接
        void RunCloseHandle();

        // 处理网络消息
        void RunTrusteeMessage();
        void RunHandleMessage();

    public:
        // 有效连接列表
        KFMap< uint32, uint32, KFNetHandle > _kf_handles;

    protected:
        // 当前时间
        uint64 _now_time;

        // 服务器服务
        KFNetServerServices* _net_server_services;

        // 等待验证的托管连接
        KFMap< uint32, uint32, KFNetHandle > _trustee_handles;

        // 删除的托管连接
        std::map< uint32, bool > _remove_trustees;

        // 需要关闭的连接
        std::map< uint32, uint64 > _close_handles;
    private:

        // 消息函数
        KFNetFunction _net_function;

        // 断开连接事件
        KFServerLostFunction _server_lost_function;
    };
}

#endif