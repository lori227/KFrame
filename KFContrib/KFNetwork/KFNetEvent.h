#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#include "KFNetDefine.h"
#include "KFBind.h"
#include "KFQueue.h"

namespace KFrame
{
    // 网络时间函数
    typedef std::function<void( const KFNetEventData* eventdata )> KFEventFunction;
    typedef std::function<void( const KFNetEventData* eventdata )> KFNetServerFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class KFNetEvent
    {
    public:
        KFNetEvent();
        virtual ~KFNetEvent();

        // 初始化
        void InitEvent( uint32 maxcount );

        // 关闭
        void ShutEvent();

        // 添加连接事件
        void AddEvent( uint32 type, uint64 id, void* data = nullptr, const char* describe = "", int32 code = 0 );

        // 事件更新
        void RunEvent();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 绑定连接事件
        template< class T >
        void BindConnectFunction( T* object, void ( T::*handle )( const KFNetEventData* eventdata ) )
        {
            _kf_connect_function = std::bind( handle, object, std::placeholders::_1 );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 绑定关闭事件
        template< class T >
        void BindDisconnectFunction( T* object, void ( T::*handle )( const KFNetEventData* eventdata ) )
        {
            _kf_disconnect_function = std::bind( handle, object, std::placeholders::_1 );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 绑定失败事件
        template< class T >
        void BindFailedFunction( T* object, void ( T::*handle )( const KFNetEventData* eventdata ) )
        {
            _kf_failed_function = std::bind( handle, object, std::placeholders::_1 );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 绑定关闭事件
        template< class T >
        void BindShutFunction( T* object, void ( T::*handle )( const KFNetEventData* eventdata ) )
        {
            _kf_shut_function = std::bind( handle, object, std::placeholders::_1 );
        }

    protected:
        // 处理绑定时间
        void HandleBindEventFunction( const KFNetEventData* eventdata );

    protected:
        // 回调函数
        KFEventFunction _kf_connect_function;
        KFEventFunction _kf_disconnect_function;
        KFEventFunction _kf_failed_function;
        KFEventFunction _kf_shut_function;

        // 事件列表
        KFQueue< KFNetEventData > _net_event_data;
    };

}

#endif