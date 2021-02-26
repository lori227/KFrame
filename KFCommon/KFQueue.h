#ifndef __KF_QUEUE_H__
#define __KF_QUEUE_H__

#include "KFDefine.h"

namespace KFrame
{
    // 环形队列, 适用于一个生产者和一个消费者的多线程情况
    template<class T>
    class KFQueue
    {
        typedef std::shared_ptr<T> ObjectPtr;
    public:
        // 析构函数
        virtual ~KFQueue()
        {
            Clear();
        }

        // 初始化队列
        // @max_count 环形队列的最大数量,
        // @extend_count 当环形队列满时,额外的链表的最大数量, 0 表示丢弃
        inline void InitQueue( uint32 max_count, uint32 extend_count )
        {
            _pop_index = 0u;
            _push_index = 0u;
            _max_count = max_count;
            _extend_count = extend_count;
            _objects.resize( max_count, nullptr );
        }

        // 最大容量
        inline uint32 Capacity() const
        {
            return _max_count;
        }

        // 队列元素数量
        inline uint32 Size() const
        {
            if ( _max_count == 0u )
            {
                return 0u;
            }

            uint32 count = 0u;
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
        inline uint32 EmptySize()
        {
            if ( _max_count == 0u )
            {
                return 0u;
            }

            return _max_count - Size();
        }

        // 判断队列是否满了
        inline bool IsFull()
        {
            if ( _max_count == 0u )
            {
                return true;
            }

            return _objects[ _push_index ] != nullptr;
        }

        // 判断队列是否为空
        inline bool IsEmpty()
        {
            return Size() == 0;
        }

        // 添加元素
        // @object : 对象指针
        bool Push( ObjectPtr object )
        {
            if ( _extends.empty() )
            {
                if ( _objects[ _push_index ] == nullptr )
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

                return false;
            }

            // 先插入列表中( todo : 如果考虑到内存问题, 需要判断_extend_count的数量限制 )
            _extends.push_back( object );

            // 判断是否环形列表是否有空位
            do
            {
                if ( _objects[ _push_index ] != nullptr )
                {
                    break;
                }

                _objects[ _push_index ] = _extends.front();
                _push_index = ( _push_index + 1 ) % _max_count;

                _extends.pop_front();
            } while ( !_extends.empty() );

            return true;
        }

        // 弹出元素
        ObjectPtr Pop()
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
        inline ObjectPtr Front()
        {
            return _objects[ _pop_index ];
        }

        inline void Clear()
        {
            _pop_index = 0;
            _push_index = 0;

            _objects.clear();
            _objects.resize( _max_count, nullptr );
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
        std::vector< ObjectPtr > _objects;

        // 额外链表的最大数量
        uint32 _extend_count = 0;
        std::list< ObjectPtr > _extends;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////


}

#endif