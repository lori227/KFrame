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
        //
        std::string _module;

        // objectid
        uint64 _object_id = 0;

        // subid
        uint64 _sub_id = 0;

        // 类型
        uint32 _type = 0;

        // 次数
        uint32 _count = 0;

        // 时间间隔
        uint32 _interval = 0;

        // 延迟时间
        uint32 _delay = 0;

        // 时间轮
        uint32 _rotation = 0;

        // 槽索引
        uint32 _slot = 0;

        // 回调函数
        KFTimerFunction _function = nullptr;

        // 链表
        KFTimerData* _prev = nullptr;
        KFTimerData* _next = nullptr;

        // 是否准备删除
        bool _is_prepare_remove = false;
    };

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFObjectData
    {
    public:
        KFObjectData() = default;
        virtual ~KFObjectData()
        {
            for ( auto& iter : _timer_list )
            {
                __KF_DELETE__( KFTimerData, iter.second );
            }
            _timer_list.clear();
        }

        inline KFTimerData* FindTimerData( uint64 id )
        {
            auto iter = _timer_list.find( id );
            if ( iter == _timer_list.end() )
            {
                return nullptr;
            }

            return iter->second;
        }

        inline bool AddTimerData( uint64 id, KFTimerData* timerdata )
        {
            return _timer_list.insert( std::make_pair( id, timerdata ) ).second;
        }

        inline bool RemoveTimerData( uint64 id )
        {
            auto iter = _timer_list.find( id );
            if ( iter == _timer_list.end() )
            {
                return false;
            }

            __KF_DELETE__( KFTimerData, iter->second );
            _timer_list.erase( iter );
            return true;
        }

    public:
        // 定时器列表
        std::unordered_map< uint64, KFTimerData* > _timer_list;
    };
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFModuleData
    {
    public:
        KFModuleData() = default;
        virtual ~KFModuleData()
        {
            for ( auto& iter : _object_list )
            {
                __KF_DELETE__( KFObjectData, iter.second );
            }
            _object_list.clear();
        }

        inline KFObjectData* FindObjectData( uint64 objectid )
        {
            auto iter = _object_list.find( objectid );
            if ( iter == _object_list.end() )
            {
                return nullptr;
            }

            return iter->second;
        }

        inline KFTimerData* FindTimerData( uint64 objectid, uint64 subid )
        {
            auto objectdata = FindObjectData( objectid );
            if ( objectdata == nullptr )
            {
                return nullptr;
            }

            return objectdata->FindTimerData( subid );
        }

        inline bool RemoveObjectData( uint64 objectid )
        {
            auto iter = _object_list.find( objectid );
            if ( iter == _object_list.end() )
            {
                return false;
            }

            __KF_DELETE__( KFObjectData, iter->second );
            _object_list.erase( iter );
            return true;
        }

        inline bool RemoveTimerData( uint64 objectid, uint64 subid )
        {
            auto objectdata = FindObjectData( objectid );
            if ( objectdata == nullptr )
            {
                return false;
            }

            return objectdata->RemoveTimerData( subid );
        }

        inline bool AddTimerData( KFTimerData* timerdata )
        {
            auto iter = _object_list.find( timerdata->_object_id );
            if ( iter == _object_list.end() )
            {
                auto objectdata = __KF_NEW__( KFObjectData );
                iter = _object_list.insert( std::make_pair( timerdata->_object_id, objectdata ) ).first;
            }

            return iter->second->AddTimerData( timerdata->_sub_id, timerdata );
        }
    public:
        std::unordered_map< uint64, KFObjectData* > _object_list;
    };
}






#endif