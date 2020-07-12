#ifndef __KF_TASK_CHAIN_CONFIG_H__
#define __KF_TASK_CHAIN_CONFIG_H__

#include "KFCore/KFCondition.h"
#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

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
    class KFTaskChainRefreshSetting : public KFIntSetting
    {
    public:
        // 任务链id
        uint32 _task_chain_id = 0u;

        // 静态刷新时间id
        uint32 _reset_refresh_time = _invalid_int;

        // 动态刷新间隔时间
        uint32 _timer_refresh_time = _invalid_int;

        // 刷新概率
        uint32 _refresh_rate = 0u;

        // 接取时间限制 单位:秒
        uint32 _receive_time = 0u;

        // 完成时间限制 单位:秒
        uint32 _done_time = 0u;

        // 接取消耗
        KFElements _receive_cost;

        // 前置条件
        KFConditions _conditions;
    };

    class KFTaskChainRefreshTimeData
    {
    public:
        ~KFTaskChainRefreshTimeData()
        {
            _refresh_list.clear();
        }

        std::vector< KFTaskChainRefreshSetting* > _refresh_list;
    };

    class KFTaskChainRefreshConfig : public KFConfigT< KFTaskChainRefreshSetting >, public KFInstance< KFTaskChainRefreshConfig >
    {
    public:
        KFTaskChainRefreshConfig()
        {
            _file_name = "taskchainrefresh";
        }

        // 加载完成
        virtual void LoadAllComplete();

    protected:
        // 清空数据
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTaskChainRefreshSetting* kfsetting );

    public:
        // 重置刷新时间表
        KFHashMap< uint32, uint32, KFTaskChainRefreshTimeData > _reset_data_list;
    };

}

#endif