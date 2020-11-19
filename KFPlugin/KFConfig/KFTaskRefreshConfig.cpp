#include "KFTaskRefreshConfig.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskRefreshConfig::ClearSetting()
    {
        _reset_data_list.Clear();
        KFConfigT< KFTaskRefreshSetting >::ClearSetting();
    }

    void KFTaskRefreshConfig::ReadSetting( KFXmlNode& xmlnode, KFTaskRefreshSetting* kfsetting )
    {
        kfsetting->_refresh_rate = xmlnode.GetUInt32( "Rate" );
        kfsetting->_task_chain_id = xmlnode.GetUInt32( "ChainId" );
        kfsetting->_receive_time = xmlnode.GetUInt32( "PickupTime" );
        kfsetting->_done_time = xmlnode.GetUInt32( "CompleteTime" );
        kfsetting->_receive_cost._str_parse = xmlnode.GetString( "PickupCost" );

        auto strtaskpool = xmlnode.GetString( "TaskPool" );
        kfsetting->_task_pool_id = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );
        kfsetting->_task_count = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );
        kfsetting->_task_status = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );

        auto strcondition = xmlnode.GetString( "Condition" );
        kfsetting->_conditions.Parse( strcondition, kfsetting->_id, __FILE__, __LINE__ );

        auto resettimeid = xmlnode.GetUInt32( "ResetTime" );
        if ( resettimeid != 0 )
        {
            kfsetting->_reset_refresh_time = resettimeid;
            auto kfrefreshdata = _reset_data_list.Create( resettimeid );
            kfrefreshdata->_refresh_list.push_back( kfsetting );
        }

        auto timertime = xmlnode.GetUInt32( "TimerTime" );
        if ( timertime )
        {
            kfsetting->_timer_refresh_time = timertime;
        }
    }

    void KFTaskRefreshConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_receive_cost, __FILE__, kfsetting->_id );
        }
    }
}