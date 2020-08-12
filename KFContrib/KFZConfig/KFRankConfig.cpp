#include "KFRankConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // 清除配置
    void KFRankConfig::ClearSetting()
    {
        _show_data_list.clear();
        _kf_rank_data_list.clear();
        KFConfigT<KFRankSetting>::ClearSetting();
    }

    void KFRankConfig::ReadSetting( KFNode& xmlnode, KFRankSetting* kfsetting )
    {
        kfsetting->_zone_type = xmlnode.GetUInt32( "ZoneType" );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_refresh_time_id = xmlnode.GetUInt32( "RefreshTimeId" );
        kfsetting->_reset_data_type = xmlnode.GetUInt32( "RefreshReset" );

        auto strcalcdata = xmlnode.GetString( "CalcData" );
        while ( !strcalcdata.empty() )
        {
            auto strdata = KFUtility::SplitString( strcalcdata, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                KFRandCalcData calcdata;
                calcdata._parent_name = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                calcdata._key = KFUtility::SplitValue<uint64>( strdata, __DOMAIN_STRING__ );
                calcdata._child_name = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                calcdata._max_value = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                kfsetting->_calc_data.emplace_back( calcdata );

                auto strkey = __FORMAT__( "{}:{}:{}", calcdata._parent_name, calcdata._key, calcdata._child_name );
                _kf_rank_data_list[ strkey ].push_back( kfsetting );
            }
        }

        if ( _show_data_list.empty() )
        {
            _show_data_list = xmlnode.GetStringSet( "ShowData" );
        }
    }

    std::vector< KFRankSetting* >& KFRankConfig::FindRankSetting( const std::string& parentname, uint64 key, const std::string& childname )
    {
        auto strkey = __FORMAT__( "{}:{}:{}", parentname, key, childname );
        auto iter = _kf_rank_data_list.find( strkey );
        if ( iter == _kf_rank_data_list.end() )
        {
            static std::vector< KFRankSetting* > _empty_list;
            return _empty_list;
        }

        return iter->second;
    }
}