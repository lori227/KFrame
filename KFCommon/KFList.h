#ifndef __KF_LIST_H__
#define __KF_LIST_H__

#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    template< class T >
    class KFList
    {
    public:
        virtual ~KFList()
        {
            Clear();
        }

        bool IsEmpty()
        {
            return _objects.empty();
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

        T* CreateBack()
        {
            auto object = __KF_NEW__( T );
            _objects.push_back( object );
            return object;
        }

        T* CreateFront()
        {
            auto object = __KF_NEW__( T );
            _objects.push_front( object );
            return object;
        }

        void Remove( T* object, bool isdelete = true )
        {
            _objects.remove( object );
            if ( isdelete )
            {
                __KF_DELETE__( T, object );
            }
        }

        T* PopBack()
        {
            if ( _objects.empty() )
            {
                return nullptr;
            }

            auto object = _objects.back();
            _objects.pop_back();
            return object;
        }

        T* PopFront()
        {
            if ( _objects.empty() )
            {
                return nullptr;
            }

            auto object = _objects.front();
            _objects.pop_front();
            return _objects;
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
        std::list< T* > _objects;

    protected:
        typename std::list< T* >::iterator _iter;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
}

#endif