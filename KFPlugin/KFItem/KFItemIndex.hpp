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
        void InitMaxIndex( uint32 maxvalue )
        {
            _emptys.clear();
            _uuids.clear();
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= maxvalue; ++i )
            {
                _emptys.insert( i );
            }
        }

        // 添加索引
        void AddMaxIndex( uint32 maxcount, uint32 addcount )
        {
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= addcount; ++i )
            {
                _emptys.insert( i + maxcount );
            }
        }

        uint32 FindEmpty( uint64 uuid )
        {
            auto iter = _emptys.begin();
            if ( iter == _emptys.end() )
            {
                return 0u;
            }

            auto index = *iter;
            _emptys.erase( iter );

            AddUUID( index, uuid );
            return index;
        }

        void AddEmpty( uint32 index )
        {
            if ( index >= __BEGIN_ITEM_INDEX__ )
            {
                _emptys.insert( index );
                RemoveUUID( index );
            }
        }

        void RemoveEmpty( uint32 index, uint64 uuid )
        {
            _emptys.erase( index );
            AddUUID( index, uuid );
        }

        bool IsEmpty( uint32 index )
        {
            return _emptys.find( index ) != _emptys.end();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        void AddUUID( uint32 index, uint64 uuid )
        {
            _uuids[ index ] = uuid;
        }

        void RemoveUUID( uint32 index )
        {
            _uuids.erase( index );
        }

        uint64 GetUUID( uint32 index )
        {
            auto iter = _uuids.find( index );
            if ( iter == _uuids.end() )
            {
                return 0u;
            }

            return iter->second;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        uint32 AddItemIndex( uint64 uuid, uint32 index, uint32 maxcount )
        {
            auto newindex = 0u;

            // 判断索引
            if ( index == 0u || index > maxcount )
            {
                // 索引为0, 直接找一个空格子
                newindex = FindEmpty( uuid );
            }
            else
            {
                auto olduuid = GetUUID( index );
                if ( olduuid == 0u )
                {
                    // 获得索引没设置uuid
                    RemoveEmpty( index, uuid );
                }
                else if ( olduuid != uuid )
                {
                    // 索引和uuid不匹配
                    newindex = FindEmpty( uuid );
                }
            }

            return newindex;
        }

        void RemoveItemIndex( uint64 uuid, uint32 index )
        {
            auto olduuid = GetUUID( index );
            if ( olduuid == uuid )
            {
                AddEmpty( index );
            }
        }

        void UpdateItemIndex( uint64 uuid, uint32 oldindex, uint32 newindex )
        {
            if ( oldindex != 0u )
            {
                RemoveItemIndex( uuid, oldindex );
            }

            RemoveEmpty( newindex, uuid );
        }

    public:
        // 名字
        std::string _name;

        // 空索引列表
        UInt32Set _emptys;

        // 索引对应的物品uuid
        UInt64Map _uuids;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    class KFItemBagIndex
    {
    public:
        // 查找页签数据
        KFItemTabIndex* FindTab( const std::string& tabname )
        {
            return _tab_index_list.Find( tabname );
        }

        void InitMaxIndex( uint32 maxcount, const StringSet& tablist )
        {
            _max_count = maxcount;
            for ( auto& tabname : tablist )
            {
                InitMaxIndex( tabname );
            }
        }

        KFItemTabIndex* InitMaxIndex( const std::string& tabname )
        {
            auto kftabindex = _tab_index_list.Create( tabname );
            kftabindex->_name = tabname;
            kftabindex->InitMaxIndex( _max_count );
            return kftabindex;
        }

        // 添加最大值
        void AddMaxIndex( uint32 count )
        {
            for ( auto& iter : _tab_index_list._objects )
            {
                auto kftabindex = iter.second;
                kftabindex->AddMaxIndex( _max_count, count );
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
        KFHashMap<std::string, const std::string&, KFItemTabIndex> _tab_index_list;
    };

}

#endif