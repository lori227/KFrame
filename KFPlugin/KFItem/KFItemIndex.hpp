#ifndef __KF_ITEM_INDEX_H__
#define __KF_ITEM_INDEX_H__
#include "KFrame.h"

namespace KFrame
{
#define __BEGIN_ITEM_INDEX__ 1u

    class KFItemTabIndex
    {
    public:
        // 初始化
        void InitMaxIndex( uint32 max_value )
        {
            _empty_list.clear();
            _uuid_list.clear();
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= max_value; ++i )
            {
                _empty_list.insert( i );
            }
        }

        // 添加索引
        void AddMaxIndex( uint32 max_count, uint32 add_count )
        {
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= add_count; ++i )
            {
                _empty_list.insert( i + max_count );
            }
        }

        uint32 FindEmpty( uint64 uuid )
        {
            auto iter = _empty_list.begin();
            if ( iter == _empty_list.end() )
            {
                return 0u;
            }

            auto index = *iter;
            _empty_list.erase( iter );

            AddUUID( index, uuid );
            return index;
        }

        void AddEmpty( uint32 index )
        {
            if ( index >= __BEGIN_ITEM_INDEX__ )
            {
                _empty_list.insert( index );
                RemoveUUID( index );
            }
        }

        void RemoveEmpty( uint32 index, uint64 uuid )
        {
            _empty_list.erase( index );
            AddUUID( index, uuid );
        }

        bool IsEmpty( uint32 index )
        {
            return _empty_list.find( index ) != _empty_list.end();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        void AddUUID( uint32 index, uint64 uuid )
        {
            _uuid_list[index] = uuid;
        }

        void RemoveUUID( uint32 index )
        {
            _uuid_list.erase( index );
        }

        uint64 GetUUID( uint32 index )
        {
            auto iter = _uuid_list.find( index );
            if ( iter == _uuid_list.end() )
            {
                return 0u;
            }

            return iter->second;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        uint32 AddItemIndex( uint64 uuid, uint32 index, uint32 max_count )
        {
            auto new_index = 0u;

            // 判断索引
            if ( index == 0u || index > max_count )
            {
                // 索引为0, 直接找一个空格子
                new_index = FindEmpty( uuid );
            }
            else
            {
                auto old_uuid = GetUUID( index );
                if ( old_uuid == 0u )
                {
                    // 获得索引没设置uuid
                    RemoveEmpty( index, uuid );
                }
                else if ( old_uuid != uuid )
                {
                    // 索引和uuid不匹配
                    new_index = FindEmpty( uuid );
                }
            }

            return new_index;
        }

        void RemoveItemIndex( uint64 uuid, uint32 index )
        {
            auto old_uuid = GetUUID( index );
            if ( old_uuid == uuid )
            {
                AddEmpty( index );
            }
        }

        void UpdateItemIndex( uint64 uuid, uint32 old_index, uint32 new_index )
        {
            if ( old_index != 0u )
            {
                RemoveItemIndex( uuid, old_index );
            }

            RemoveEmpty( new_index, uuid );
        }

    public:
        // 名字
        std::string _name;

        // 空索引列表
        UInt32Set _empty_list;

        // 索引对应的物品uuid
        UInt64Map _uuid_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    class KFItemBagIndex
    {
    public:
        // 查找页签数据
        std::shared_ptr<KFItemTabIndex> FindTab( const std::string& tab_name )
        {
            return _tab_index_list.Find( tab_name );
        }

        void InitMaxIndex( uint32 max_count, const StringSet& tab_list )
        {
            _max_count = max_count;
            for ( auto& tab_name : tab_list )
            {
                InitMaxIndex( tab_name );
            }
        }

        std::shared_ptr<KFItemTabIndex> InitMaxIndex( const std::string& tab_name )
        {
            auto tab_index = _tab_index_list.Create( tab_name );
            tab_index->_name = tab_name;
            tab_index->InitMaxIndex( _max_count );
            return tab_index;
        }

        // 添加最大值
        void AddMaxIndex( uint32 count )
        {
            for ( auto& iter : _tab_index_list._objects )
            {
                iter.second->AddMaxIndex( _max_count, count );
            }

            _max_count += count;
        }

        // 最大索引
        uint32 GetMaxIndex() const
        {
            return _max_count;
        }

        /////////////////////////////////////////////////////////////////////////////////
    public:
        // 最大数量
        uint32 _max_count = 0u;

        // 页签列表
        KFHashMap<std::string, KFItemTabIndex> _tab_index_list;
    };

}

#endif