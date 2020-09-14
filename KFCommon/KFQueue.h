#ifndef __KF_QUEUE_H__
#define __KF_QUEUE_H__

#include "KFInclude.h"
#include "KFUtility/KFAppId.h"
#include "KFMemory/KFMalloc.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
    // 环形队列, 适用于一个生产者和一个消费者的多线程情况
    template< class T, bool _need_to_delete = true >
    class KFQueue
    {
    public:
        KFQueue()
        {
            _push_index = 0;
            _pop_index = 0;
            _max_count = 0;
            _objects.clear();
        }

        ~KFQueue()
        {
            ClearObject();
        }

        // maxcount 环形队列的最大数量,
        // extendcount 当环形队列满时,额外的链表的最大数量, 0 表示丢弃
        void InitQueue( uint32 maxcount, uint32 extendcount )
        {
            _max_count = maxcount;
            _extend_count = extendcount;
            _objects.resize( maxcount, nullptr );
        }

        uint32 Capacity()
        {
            return _max_count;
        }

        // 队列元素数量
        uint32 Size()
        {
            if ( _max_count == 0 )
            {
                return 0;
            }

            uint32 count = 0;
            if ( _push_index > _pop_index )
            {
                count = _push_index - _pop_index;
            }
            else if ( _push_index < _pop_index )
            {
                count = ( _max_count + _push_index ) - _pop_index;
            }

            return count;
        }

        // 空闲元素数量
        uint32 EmptySize()
        {
            if ( _max_count == 0 )
            {
                return 0;
            }

            return _max_count - Size();
        }

        // 判断队列是否满了
        bool IsFull()
        {
            if ( _max_count == 0 )
            {
                return true;
            }

            auto oldobject = _objects[ _push_index ];
            return oldobject != nullptr;
        }

        // 判断队列是否为空
        bool IsEmpty()
        {
            return Size() == 0;
        }

        // 添加元素
        bool PushObject( T* object, uint64 parentid, const char* function, uint32 line )
        {
            if ( _extends.empty() )
            {
                auto oldobject = _objects[ _push_index ];
                if ( oldobject == nullptr )
                {
                    // 列表未满, 直接插入
                    _objects[ _push_index ] = object;
                    _push_index = ( _push_index + 1 ) % _max_count;
                    return true;
                }

                // 列表满了, 判断是否需要插入额外列表
                if ( _extend_count > 0u )
                {
                    _extends.push_back( object );
                    return true;
                }

                // 不能插入push了,否则覆盖会造成内存泄漏
                if ( _need_to_delete )
                {
                    __KF_DELETE__( T, object );
                }
                return false;
            }

            // 先插入列表中( todo : 如果考虑到内存问题, 需要判断_extend_count的数量限制 )
            _extends.push_back( object );

            // 需要日志报警
            if ( _extends.size() % 100u == 0 )
            {
                __LOG_WARN_FUNCTION__( function, line, "parent=[{}:{}] queue object list extend size={}",
                                       parentid, KFAppId::ToString( parentid ), _extends.size() );
            }

            // 判断是否环形列表是否有空位
            do
            {
                auto oldobject = _objects[ _push_index ];
                if ( oldobject != nullptr )
                {
                    break;
                }

                auto popobject = _extends.front();
                _extends.pop_front();

                _objects[ _push_index ] = popobject;
                _push_index = ( _push_index + 1 ) % _max_count;
            } while ( !_extends.empty() );

            return true;
        }

        // 弹出元素
        T* PopObject()
        {
            auto object = _objects[ _pop_index ];
            if ( object != nullptr )
            {
                _objects[ _pop_index ] = nullptr;
                _pop_index = ( _pop_index + 1 ) % _max_count;
            }

            return object;
        }

        // 取第一个元素( 不删除 )
        T* Front()
        {
            return _objects[ _pop_index ];
        }

        // 删除掉一个元素
        void PopRemove()
        {
            auto object = _objects[ _pop_index ];
            if ( object == nullptr )
            {
                return;
            }

            if ( _need_to_delete )
            {
                __KF_DELETE__( T, object );
            }

            _objects[ _pop_index ] = nullptr;
            _pop_index = ( _pop_index + 1 ) % _max_count;
        }

        void ClearObject()
        {
            for ( auto i = 0u; i < _max_count; ++i )
            {
                auto object = _objects[ i ];
                if ( object != nullptr )
                {
                    if ( _need_to_delete )
                    {
                        __KF_DELETE__( T, object );
                    }
                    _objects[ i ] = nullptr;
                }
            }

            _pop_index = 0;
            _push_index = 0;

            if ( _need_to_delete )
            {
                for ( auto object : _extends )
                {
                    __KF_DELETE__( T, object );
                }
            }
            _extends.clear();
        }

    private:
        // 当前push的索引
        volatile uint32 _push_index;

        // 当前pop的索引
        volatile uint32 _pop_index;

        // 最大数量
        volatile uint32 _max_count;

        // 列表
        std::vector< T* > _objects;

        // 额外链表的最大数量
        uint32 _extend_count = 0;
        std::list< T* > _extends;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////


}

#endif