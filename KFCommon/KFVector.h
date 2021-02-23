#ifndef __KF_VECTOR_H__
#define __KF_VECTOR_H__

#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    template<class T>
    class KFVector
    {
        typedef std::shared_ptr<T> ObjectPtr;
        typedef std::shared_ptr<const T> ConstObjectPtr;

    public:
        virtual ~KFVector()
        {
            Clear();
        }

        // 清空
        inline virtual void Clear()
        {
            _objects.clear();
        }

        // 重置数量
        // @max_count : 最大容量
        inline void Resize( uint32 max_count )
        {
            auto count = _objects.size();
            if ( max_count <= count )
            {
                return;
            }

            _objects.resize( max_count, nullptr );
        }

        // 查找空位置
        inline uint32 FindEmpty()
        {
            for ( auto i = 0u; i < _objects.size(); ++i )
            {
                if ( _objects[ i ] == nullptr )
                {
                    return i;
                }
            }

            return MaxSize();
        }

        // 最大数量
        inline uint32 MaxSize() const
        {
            return static_cast<uint32>( _objects.size() );
        }

        // 是否为空
        // @index : 索引
        inline bool IsValid( uint32 index ) const
        {
            return index < MaxSize();
        }

        // 添加
        // @object : 数据对象
        inline void Add( ObjectPtr& object )
        {
            uint32 index = FindEmpty();
            Insert( index, object );
        }

        // 插入
        // index : 索引
        // @object : 数据对象
        inline void Insert( uint32 index, ObjectPtr& object )
        {
            // 确保容量大小
            Resize( index+ 1 );
            _objects[ index ] = object;
        }

        // 查找
        inline ObjectPtr Find( uint32 index ) const
        {
            if ( !IsValid( index ) )
            {
                return nullptr;
            }

            return _objects[ index ];
        }

        // 删除
        inline ObjectPtr Remove( uint32 index )
        {
            if ( !IsValid( index ) )
            {
                return nullptr;
            }

            auto object = _objects[ index ];
            _objects[ index ] = nullptr;
            return object;
        }

        inline ObjectPtr First()
        {
            _iter = _objects.begin();
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return *_iter;
        }

        inline ObjectPtr Next()
        {
            ++_iter;
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return *_iter;
        }

    public:
        // 列表
        std::vector< ObjectPtr > _objects;

    protected:
        typename std::vector< ObjectPtr >::iterator _iter;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
}

#endif