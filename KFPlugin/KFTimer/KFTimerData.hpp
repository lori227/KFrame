#ifndef __KF_TIMER_DATA_H__
#define __KF_TIMER_DATA_H__

#include "KFTimerInterface.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFTimerData
    {
    public:
        // objectid
        uint64 _object_id = 0;

        // 数据id
        uint64 _data_id = 0;

        // 类型
        uint32 _type = 0;

        // 总共的次数
        uint32 _count = 0;

        // 执行的次数
        uint32 _turns = 0;

        // 时间间隔
        uint32 _interval = 0;

        // 延迟时间
        uint32 _delay = 0;

        // 时间轮
        uint32 _rotation = 0;

        // 槽索引
        uint32 _slot = 0;

        // 回调函数
        KFModuleFunction<KFTimerFunction> _function;

        // 链表
        std::shared_ptr<KFTimerData> _prev = nullptr;
        std::shared_ptr<KFTimerData> _next = nullptr;

        // 是否准备删除
        bool _is_prepare_remove = false;
    };

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFObjectTimerData
    {
    public:
        KFObjectTimerData() = default;
        virtual ~KFObjectTimerData()
        {
            _timer_list.Clear();
        }

        inline std::shared_ptr<KFTimerData> FindTimerData( uint64 data_id )
        {
            return _timer_list.Find( data_id );
        }

        inline bool AddTimerData( uint64 data_id, std::shared_ptr<KFTimerData> timer_data )
        {
            _timer_list.Insert( data_id, timer_data );
            return true;
        }

        inline std::shared_ptr<KFTimerData> RemoveTimerData( uint64 data_id )
        {
            return _timer_list.Remove( data_id );
        }

    public:
        // 定时器列表
        KFHashMap<uint64, KFTimerData> _timer_list;
    };
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFModuleTimerData
    {
    public:
        KFModuleTimerData() = default;
        virtual ~KFModuleTimerData()
        {
            _object_list.Clear();
        }

        inline std::shared_ptr<KFObjectTimerData> FindObjectData( uint64 object_id )
        {
            return _object_list.Find( object_id );
        }

        inline std::shared_ptr<KFTimerData> FindTimerData( uint64 object_id, uint64 data_id )
        {
            auto object_data = FindObjectData( object_id );
            if ( object_data == nullptr )
            {
                return nullptr;
            }

            return object_data->FindTimerData( data_id );
        }

        inline std::shared_ptr<KFObjectTimerData> RemoveObjectData( uint64 object_id )
        {
            return _object_list.Remove( object_id );
        }

        inline std::shared_ptr<KFTimerData> RemoveTimerData( uint64 object_id, uint64 data_id )
        {
            auto object_data = FindObjectData( object_id );
            if ( object_data == nullptr )
            {
                return nullptr;
            }

            return object_data->RemoveTimerData( data_id );
        }

        inline bool AddTimerData( std::shared_ptr<KFTimerData> timer_data )
        {
            auto object_data = _object_list.Create( timer_data->_object_id );
            object_data->AddTimerData( timer_data->_data_id, timer_data );
        }
    public:
        KFHashMap<uint64, KFObjectTimerData> _object_list;
    };
}






#endif