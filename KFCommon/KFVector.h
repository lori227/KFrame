#ifndef __KF_VECTOR_H__
#define __KF_VECTOR_H__

#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    template< class T >
    class KFVector
    {
    public:
        KFVector()
        {
            _objects.clear();
        }

        virtual ~KFVector()
        {
            Clear();
        }

        // 清空
        void Clear()
        {
            for ( auto object : _objects )
            {
                __KF_DELETE__( T, object );
            }

            _objects.clear();
        }

        // 重置数量
        void Resize( uint32 maxcount )
        {
            auto count = _objects.size();
            if ( maxcount <= count )
            {
                return;
            }

            _objects.resize( maxcount, nullptr );
        }

        // 空位置
        uint32 FindEmpty()
        {
            uint32 index = 0;
            for ( auto* object : _objects )
            {
                if ( object == nullptr )
                {
                    return index;
                }

                ++index;
            }

            return MaxSize();
        }

        // 最大数量
        uint32 MaxSize()
        {
            return static_cast< uint32 >( _objects.size() );
        }

        // 是否为空
        bool IsValid( uint32 index )
        {
            return index < MaxSize();
        }

        // 添加
        void Add( T* object )
        {
            _objects.push_back( object );
        }

        // 添加
        void Insert( T* object )
        {
            uint32 index = FindEmpty();
            Insert( index, object );
        }

        void Insert( uint32 index, T* object )
        {
            if ( !IsValid( index ) )
            {
                _objects.resize( index + 1, nullptr );
            }

            _objects[ index ] = object;
        }

        // 查找
        T* Find( uint32 index )
        {
            if ( !IsValid( index ) )
            {
                return nullptr;
            }

            return _objects[ index ];
        }

        // 删除
        void Remove( uint32 index )
        {
            if ( !IsValid( index ) )
            {
                return;
            }

            auto* object = _objects[ index ];
            __KF_DELETE__( T, object );
            _objects[ index ] = nullptr;
        }

        T* First()
        {
            _iter = _objects.begin();
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return *_iter;
        }

        T* Next()
        {
            ++_iter;
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return *_iter;
        }

        bool End()
        {
            return _iter == _objects.end();

        }

    public:
        // 列表
        std::vector< T* > _objects;

    protected:
        typename std::vector< T* >::iterator _iter;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
}

#endif