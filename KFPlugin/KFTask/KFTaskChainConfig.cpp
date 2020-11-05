#include "KFTaskChainConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    const KFTaskData* KFTaskChainSetting::FindTaskData( uint32 order, uint32 taskid ) const
    {
        auto eventlist = _task_data_list.Find( order );
        if ( eventlist == nullptr )
        {
            return nullptr;
        }

        for ( auto eventdata : eventlist->_weight_data )
        {
            if ( eventdata->_id == taskid )
            {
                return eventdata;
            }
        }

        return nullptr;
    }

    void KFTaskChainConfig::ReadSetting( KFNode& xmlnode, KFTaskChainSetting* kfsetting )
    {
        auto order = xmlnode.GetUInt32( "Order" );
        auto taskdatalist = kfsetting->_task_data_list.Find( order );
        if ( taskdatalist == nullptr )
        {
            taskdatalist = __KF_NEW__( KFWeightList< KFTaskData > );
            kfsetting->_task_data_list.Insert( order, taskdatalist );
        }

        auto taskid = xmlnode.GetUInt32( "Task" );
        auto weight = xmlnode.GetUInt32( "Weight", true );
        auto taskdata = taskdatalist->Create( taskid, ( weight == 0u ? 100u : weight ) );

        taskdata->_task_status = xmlnode.GetUInt32( "TaskStatus" );

        auto strextend = xmlnode.GetString( "ExtendChain" );
        KFReadSetting::ParseMapUInt32( strextend, taskdata->_extend_task_chain_list );

        auto strstartrefresh = xmlnode.GetString( "StartRefresh" );
        KFReadSetting::ParseMapUInt32( strstartrefresh, taskdata->_start_refresh_id_list );

        auto strstoprefresh = xmlnode.GetString( "StopRefresh" );
        KFReadSetting::ParseMapUInt32( strstoprefresh, taskdata->_stop_refresh_id_list );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}