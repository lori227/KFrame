#ifndef __KF_TASK_CHAIN_CONFIG_H__
#define __KF_TASK_CHAIN_CONFIG_H__

#include "KFCore/KFCondition.h"
#include "KFZConfig/KFConfig.h"

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
        const KFTaskData* FindTaskData( uint32 order, uint32 taskid ) const;

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
        virtual void ReadSetting( KFNode& xmlnode, KFTaskChainSetting* kfsetting );
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif