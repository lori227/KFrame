#ifndef __KF_TASK_CHAIN_CONFIG_H__
#define __KF_TASK_CHAIN_CONFIG_H__

#include "KFCore/KFCondition.h"
#include "KFConfig.h"
#include "KFReadSetting.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFTaskData : public KFWeight
    {
    public:
        // 任务状态
        uint32 _task_status = 0u;

        // 附加的任务链
        UInt32Map _extend_task_chain_list;

        // 附加开启的刷新链
        UInt32Map _start_refresh_id_list;

        // 附加关闭的刷新链
        UInt32Map _stop_refresh_id_list;

    };

    class KFTaskChainSetting : public KFIntSetting
    {
    public:
        const KFTaskData* FindTaskData( uint32 order, uint32 taskid ) const
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

    public:
        // 任务数据
        KFVector< KFWeightList< KFTaskData > > _task_data_list;
    };

    class KFTaskChainConfig : public KFConfigT< KFTaskChainSetting >, public KFInstance< KFTaskChainConfig >
    {
    public:
        KFTaskChainConfig()
        {
            _file_name = "taskchain";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTaskChainSetting* kfsetting )
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
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif