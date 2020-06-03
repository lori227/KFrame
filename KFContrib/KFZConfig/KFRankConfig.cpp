#include "KFRankConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // 清除配置
    void KFRankConfig::ClearSetting()
    {
        _kf_rank_data_list.clear();
        KFConfigT<KFRankSetting>::ClearSetting();
    }

    void KFRankConfig::ReadSetting( KFNode& xmlnode, KFRankSetting* kfsetting )
    {
        kfsetting->_zone_type = xmlnode.GetUInt32( "ZoneType" );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_refresh_type = xmlnode.GetUInt32( "RefreshType" );
        kfsetting->_refresh_time = xmlnode.GetUInt32( "RefreshTime" );
        kfsetting->_refresh_hour = xmlnode.GetUInt32( "RefreshHour" );
        kfsetting->_refresh_minute = xmlnode.GetUInt32( "RefreshMinute" );
        kfsetting->_reset_data_type = xmlnode.GetUInt32( "RefreshReset" );

        auto strcalcdata = xmlnode.GetString( "CalcData" );
        while ( !strcalcdata.empty() )
        {
            auto strdata = KFUtility::SplitString( strcalcdata, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                KFRandCalcData calcdata;
                calcdata._parent_name = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                calcdata._child_name = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                calcdata._max_value = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                kfsetting->_calc_data.emplace_back( calcdata );

                RankDataType key( calcdata._parent_name, calcdata._child_name );
                _kf_rank_data_list[ key ].push_back( kfsetting );
            }
        }
    }

    std::vector< KFRankSetting* >& KFRankConfig::FindRankSetting( const std::string& parentname, const std::string& childname )
    {
        RankDataType key( parentname, childname );
        auto iter = _kf_rank_data_list.find( key );
        if ( iter == _kf_rank_data_list.end() )
        {
            static std::vector< KFRankSetting* > _empty_list;
            return _empty_list;
        }

        return iter->second;
    }
}