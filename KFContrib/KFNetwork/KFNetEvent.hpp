#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#include "KFNetDefine.hpp"
#include "KFQueue.h"
#include "KFFunction.h"

namespace KFrame
{
    // 网络时间函数
    typedef std::function<void( std::shared_ptr<KFNetEventData>& )> KFEventFunction;
    typedef std::function<void( std::shared_ptr<KFNetEventData>& )> KFNetServerFunction;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class KFNetEvent
    {
    public:
        KFNetEvent() = default;
        virtual ~KFNetEvent() = default;

        // 初始化
        void InitEvent( uint32 max_count );

        // 关闭
        void ShutEvent();

        // 添加连接事件
        void AddEvent( uint32 type, uint64 id, std::shared_ptr<void> data = nullptr, const char* describe = "", int32 code = 0 );

        // 事件更新
        void RunEvent();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 绑定事件函数
        template<class T>
        void BindEventFunction( uint32 type, T* object, void ( T::*handle )( std::shared_ptr<KFNetEventData>& ) )
        {
            KFEventFunction function = std::bind( handle, object, std::placeholders::_1 );

            auto event_function = _event_function_list.Create( type );
            event_function->SetFunction( function );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理绑定时间
        void HandleBindEventFunction( std::shared_ptr<KFNetEventData>& event_data );

    protected:
        // 事件列表
        KFQueue< KFNetEventData > _event_data_queue;

        // 回调函数
        KFMapFunction<uint32, KFEventFunction> _event_function_list;
    };

}

#endif