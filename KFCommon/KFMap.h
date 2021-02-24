#ifndef __KF_MAP_H__
#define __KF_MAP_H__

#include "KFDefine.h"

namespace KFrame
{
    template < typename KeyType, typename ObjectType, typename MapType >
    class KFMapT
    {
        typedef std::shared_ptr<ObjectType> ObjectPtr;
    public:
        KFMapT()
        {
            _objects.clear();
        }

        virtual ~KFMapT()
        {
            _objects.clear();
        }

        // 大小
        inline uint32 Size()
        {
            return static_cast<uint32>( _objects.size() );
        }

        // 清除
        inline void Clear()
        {
            _objects.clear();
        }

        // 是否为空
        inline bool IsEmpty()
        {
            return _objects.empty();
        }

        // 添加
        // @key : 主键索引
        // @object : 数据对象
        inline void Insert( const KeyType& key, ObjectPtr object )
        {
            _objects[ key ] = object;
        }

        // 查找
        // @key : 主键索引
        inline ObjectPtr Find( const KeyType& key ) const
        {
            auto iter = _objects.find( key );
            if ( iter == _objects.end() )
            {
                return nullptr;
            }

            return iter->second;
        }

        // 创建
        // @key : 主键索引
        // params : 参数列表
        template<class...P>
        inline ObjectPtr Create( const KeyType& key, P&& ... params )
        {
            auto object = Find( key );
            if ( object != nullptr )
            {
                return object;
            }

            object = std::make_shared<ObjectType>( std::forward<P>( params )... );
            _objects[ key ] = object;
            return object;
        }

        // 存在
        // @key : 主键索引
        inline bool IsExist( const KeyType& key )
        {
            return Find( key ) != nullptr;
        }

        // 删除
        // @key : 主键索引
        inline ObjectPtr Remove( const KeyType& key )
        {
            auto iter = _objects.find( key );
            if ( iter == _objects.end() )
            {
                return nullptr;
            }

            auto object = iter->second;
            _objects.erase( iter );
            return object;
        }

        // 遍历
        // 不能嵌套遍历, 中间不能有删除添加操作
        inline ObjectPtr First()
        {
            if ( _objects.empty() )
            {
                return nullptr;
            }

            _iter = _objects.begin();
            return _iter->second;
        }

        inline ObjectPtr Next()
        {
            ++_iter;
            if ( _iter == _objects.end() )
            {
                return nullptr;
            }

            return _iter->second;
        }

        // 添加数据
        void AddMap( KFMapT& other )
        {
            for ( auto iter : other._objects )
            {
                Insert( iter.first, iter.second );
            }

            other.Clear();
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
    template< typename KeyType, typename ObjectType >
    class KFMap : public KFMapT<KeyType, ObjectType, std::map<KeyType, std::shared_ptr<ObjectType>>>
    {

    };

    /////////////////////////////////////////////////////////////////////////////////////
    template< typename KeyType, typename ObjectType >
    class KFHashMap : public KFMapT<KeyType, ObjectType, std::unordered_map<KeyType, std::shared_ptr<ObjectType>>>
    {

    };
}
#endif
