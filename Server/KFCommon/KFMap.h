#ifndef __KF_MAP_H__
#define __KF_MAP_H__

#include "KFInclude.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    template< typename KeyType, typename ParamType, typename ObjectType >
    class KFMap
    {
    public:
        KFMap()
        {
            _objects.clear();
        }

        virtual ~KFMap()
        {
            Clear();
        }

        // 大小
        uint32 Size()
        {
            return static_cast< uint32 >( _objects.size() );
        }

        // 添加
        void Insert( ParamType key, ObjectType* object )
        {
            Remove( key, true );
            _objects[ key ] = object;
        }

        // 查找
        ObjectType* Find( ParamType key ) const
        {
            auto iter = _objects.find( key );
            if ( iter == _objects.end() )
            {
                return nullptr;
            }

            return iter->second;
        }

        ObjectType* Create( ParamType key )
        {
            auto object = Find( key );
            if ( object != nullptr )
            {
                return object;
            }

            object = __KF_CREATE__( ObjectType );
            Insert( key, object );
            return object;
        }

        // 存在
        bool IsExist( ParamType key )
        {
            return Find( key ) != nullptr;
        }

        // 删除
        bool Remove( ParamType key, bool isdelete = true )
        {
            auto iter = _objects.find( key );
            if ( iter == _objects.end() )
            {
                return false;
            }

            if ( isdelete )
            {
                __KF_DESTROY__( ObjectType, iter->second );
            }

            _objects.erase( iter );
            return true;
        }

        // 清除
        void Clear( bool isdelete = true )
        {
            if ( isdelete )
            {
                for ( auto iter = _objects.begin(); iter != _objects.end(); ++iter )
                {
                    __KF_DESTROY__( ObjectType, iter->second );
                }
            }

            _objects.clear();
        }

        // 是否为空
        bool IsEmpty()
        {
            return _objects.empty();
        }

        // 遍历
        // 不能嵌套遍历, 中间不能有删除添加操作
        ObjectType* First()
        {
            if ( _objects.empty() )
            {
                return nullptr;
            }

            _iter = _objects.begin();
            return _iter->second;
        }

        ObjectType* Next()
        {
            ++_iter;
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return _iter->second;
        }

        // 添加数据
        void AddMap( KFMap& kfother )
        {
            for ( auto iter : kfother._objects )
            {
                Insert( iter.first, iter.second );
            }

            kfother._objects.clear();
        }

    public:
        // 迭代器
        typename std::map< KeyType, ObjectType* >::iterator _iter;

        // 列表
        typename std::map< KeyType, ObjectType* > _objects;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
}
#endif
