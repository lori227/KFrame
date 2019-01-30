#ifndef __KF_QUEUE_H__
#define __KF_QUEUE_H__

#include "KFInclude.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    // 环形队列, 适用于一个生产者和一个消费者的多线程情况
    template< class T >
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

        // 初始化
        void InitQueue( uint32 maxcount )
        {
            Resize( maxcount );
        }

        void Resize( uint32 maxcount )
        {
            if ( maxcount > _max_count )
            {
                _max_count = maxcount;
                _objects.resize( maxcount, nullptr );
            }
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

            auto* oldobject = _objects[ _push_index ];
            return oldobject != nullptr;
        }

        // 判断队列是否为空
        bool IsEmpty()
        {
            return Size() == 0;
        }

        // 添加元素
        bool PushObject( T* object )
        {
            // 如果还有数据, 不能再push了,否则覆盖会造成内存泄漏
            auto* oldobject = _objects[ _push_index ];
            if ( oldobject != nullptr )
            {
                __KF_DELETE__( T, object );
                return false;
            }

            _objects[ _push_index ] = object;
            _push_index = ( _push_index + 1 ) % _max_count;
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
            auto* object = _objects[ _pop_index ];
            if ( object == nullptr )
            {
                return;
            }

            __KF_DELETE__( T, object );

            _objects[ _pop_index ] = nullptr;
            _pop_index = ( _pop_index + 1 ) % _max_count;
        }

        void ClearObject()
        {
            for ( auto i = 0u; i < _max_count; ++i  )
            {
                auto object = _objects[ i ];
                if ( object != nullptr )
                {
                    __KF_DELETE__( T, object );
                    _objects[ i ] = nullptr;
                }
            }

            _pop_index = 0;
            _push_index = 0;
            _max_count = 0;
            _objects.clear();
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
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////


}

#endif