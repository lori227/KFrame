#ifndef __KF_MAP_H__
#define __KF_MAP_H__

#include "KFInclude.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    template < typename KeyType, typename ParamType, typename ObjectType, typename MapType >
    class KFMapT
    {
    public:
        KFMapT()
        {
            _objects.clear();
        }

        virtual ~KFMapT()
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

            object = __KF_NEW__( ObjectType );
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
                __KF_DELETE__( ObjectType, iter->second );
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
                    __KF_DELETE__( ObjectType, iter->second );
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
        void AddMap( KFMapT& kfother )
        {
            for ( auto iter : kfother._objects )
            {
                Insert( iter.first, iter.second );
            }

            kfother._objects.clear();
        }

    public:
        // 列表
        MapType _objects;

    private:
        // 迭代器
        typename MapType::iterator _iter;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    template< typename KeyType, typename ParamType, typename ObjectType >
    class KFMap : public KFMapT < KeyType, ParamType, ObjectType, std::map< KeyType, ObjectType* > >
    {

    };

    /////////////////////////////////////////////////////////////////////////////////////
    template< typename KeyType, typename ParamType, typename ObjectType >
    class KFHashMap : public KFMapT < KeyType, ParamType, ObjectType, std::unordered_map< KeyType, ObjectType* > >
    {

    };
}
#endif
