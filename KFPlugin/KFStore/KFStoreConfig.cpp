#include "KFStoreConfig.hpp"

namespace KFrame
{
    void KFStoreConfig::ReadSetting( KFNode& xmlnode, KFStoreSetting* kfsetting )
    {
        kfsetting->_store_type = xmlnode.GetUInt32( "StoreType" );
        kfsetting->_data_name = xmlnode.GetString( "DataName" );

        auto strbuyelement = xmlnode.GetString( "BuyElement" );
        kfsetting->_buy_elements.Parse( strbuyelement, __FUNC_LINE__ );

        kfsetting->_buy_max_count = xmlnode.GetUInt32( "BuyMaxCount" );

        // 消耗花费
        auto strcostelement = xmlnode.GetString( "CostElement" );
        while ( strcostelement.empty() )
        {
            auto costelement = KFUtility::SplitString( strcostelement, "|" );

            auto name = KFUtility::SplitString( costelement, "=" );
            auto elements = kfsetting->_cost_elements.Create( name );
            elements->Parse( costelement, __FUNC_LINE__ );
        }

        // 折扣时间
        kfsetting->_start_discount_time = KFDate::FromString( xmlnode.GetString( "StartDiscountTime" ) );
        kfsetting->_end_discount_time = KFDate::FromString( xmlnode.GetString( "EndDiscountTime" ) );

        // 折扣花费
        auto strdiscountelement = xmlnode.GetString( "CostElement" );
        while ( strdiscountelement.empty() )
        {
            auto costelement = KFUtility::SplitString( strdiscountelement, "|" );

            auto name = KFUtility::SplitString( costelement, "=" );
            auto elements = kfsetting->_discount_elements.Create( name );
            elements->Parse( costelement, __FUNC_LINE__ );
        }

        auto strbuylimit = xmlnode.GetString( "BuyLimit" );
        kfsetting->_buy_limit_type = KFUtility::SplitString( strbuylimit, "," );
        kfsetting->_buy_limit_count = KFUtility::SplitValue< uint32 >( strbuylimit, "," );
        kfsetting->_max_own_count = xmlnode.GetUInt32( "MaxOwnCount" );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFElements* KFStoreSetting::FindCostElements( const std::string& buytype, uint64 nowtime ) const
    {
        // 判断是否在折扣时间内 返回折扣价
        if ( CheckInDiscount( nowtime ) )
        {
            return _discount_elements.Find( buytype );
        }

        return _cost_elements.Find( buytype );
    }

    bool KFStoreSetting::CheckInDiscount( uint64 nowtime ) const
    {
        if ( _start_discount_time == _invalid_int || _end_discount_time == _invalid_int )
        {
            return false;
        }

        return KFDate::CheckInTime( _start_discount_time, _end_discount_time, nowtime );
    }
}