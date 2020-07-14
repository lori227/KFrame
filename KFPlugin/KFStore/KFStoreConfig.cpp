#include "KFStoreConfig.hpp"
#include "KFGoodsConfig.hpp"

namespace KFrame
{
    void KFStoreConfig::ReadSetting( KFNode& xmlnode, KFStoreSetting* kfsetting )
    {
        kfsetting->_refresh_type = xmlnode.GetUInt32( "RefreshType" );
        kfsetting->_refresh_time_id = xmlnode.GetUInt32( "RefreshTimeId" );
        kfsetting->_random_type = xmlnode.GetUInt32( "RandomType" );
        kfsetting->_is_refresh_reset_time = xmlnode.GetBoolen( "RefreshResetTime" );

        auto strrefreshgoods = xmlnode.GetString( "RefreshGoods" );
        while ( !strrefreshgoods.empty() )
        {
            auto strrefresh = KFUtility::SplitString( strrefreshgoods, __SPLIT_STRING__ );
            if ( !strrefresh.empty() )
            {
                auto groupid = KFUtility::SplitValue<uint32>( strrefresh, __DOMAIN_STRING__ );
                auto mincount = KFUtility::SplitValue<uint32>( strrefresh, __RANGE_STRING__ );
                auto maxcount = KFUtility::SplitValue<uint32>( strrefresh, __RANGE_STRING__ );
                if ( maxcount < mincount )
                {
                    maxcount = mincount;
                }

                kfsetting->_refresh_group_count.emplace_back( std::make_tuple( groupid, mincount, maxcount ) );
            }
        }

        kfsetting->_refresh_cost_list = xmlnode.GetUInt32Vector( "RefreshCost" );
    }

    void KFStoreConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            kfsetting->_goods_list.clear();
        }

        // 遍历商品列表
        for ( auto& iter : KFGoodsConfig::Instance()->_settings._objects )
        {
            auto kfgoodssetting = iter.second;

            auto kfstoresetting = _settings.Find( kfgoodssetting->_store_id );
            if ( kfstoresetting != nullptr )
            {
                kfstoresetting->_goods_list.insert( kfgoodssetting->_id );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFStoreSetting::IsHaveGoods( uint32 goodsid ) const
    {
        return _goods_list.find( goodsid ) != _goods_list.end();
    }
}