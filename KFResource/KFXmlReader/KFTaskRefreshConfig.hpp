#ifndef __KF_TASK_REFRESH_CONFIG_H__
#define __KF_TASK_REFRESH_CONFIG_H__

#include "KFConfig.h"
#include "KFXmlReader/KFElementConfig.h"
#include "KFXmlReader/KFWeightConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFTaskRefreshSetting : public KFIntSetting
    {
    public:
        // 任务链id
        uint32 _task_chain_id = 0u;

        // 随机任务池id
        uint32 _task_pool_id = 0u;
        // 随机任务数量
        uint32 _task_count = 0u;
        uint32 _task_status = 0u;

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

    class KFTaskRefreshTimeData
    {
    public:
        ~KFTaskRefreshTimeData()
        {
            _refresh_list.clear();
        }

        std::vector< KFTaskRefreshSetting* > _refresh_list;
    };

    class KFTaskRefreshConfig : public KFConfigT< KFTaskRefreshSetting >, public KFInstance< KFTaskRefreshConfig >
    {
    public:
        KFTaskRefreshConfig()
        {
            _file_name = "taskrefresh";
        }

        // 加载完成
        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                KFElementConfig::Instance()->ParseElement( kfsetting->_receive_cost, __FILE__, kfsetting->_id );
            }
        }

    protected:
        // 清空数据
        virtual void ClearSetting()
        {
            _reset_data_list.Clear();
            KFConfigT< KFTaskRefreshSetting >::ClearSetting();
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFTaskRefreshSetting* kfsetting )
        {
            kfsetting->_refresh_rate = xmlnode.ReadUInt32( "Rate" );
            kfsetting->_task_chain_id = xmlnode.ReadUInt32( "ChainId" );
            kfsetting->_receive_time = xmlnode.ReadUInt32( "PickupTime" );
            kfsetting->_done_time = xmlnode.ReadUInt32( "CompleteTime" );
            kfsetting->_receive_cost._str_parse = xmlnode.ReadString( "PickupCost" );

            auto strtaskpool = xmlnode.ReadString( "TaskPool" );
            kfsetting->_task_pool_id = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );
            kfsetting->_task_count = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );
            kfsetting->_task_status = KFUtility::SplitValue<uint32>( strtaskpool, __SPLIT_STRING__ );

            xmlnode.ReadStaticCondition( kfsetting->_conditions, "Condition" );

            auto resettimeid = xmlnode.ReadUInt32( "ResetTime" );
            if ( resettimeid != 0 )
            {
                kfsetting->_reset_refresh_time = resettimeid;
                auto kfrefreshdata = _reset_data_list.Create( resettimeid );
                kfrefreshdata->_refresh_list.push_back( kfsetting );
            }

            auto timertime = xmlnode.ReadUInt32( "TimerTime" );
            if ( timertime )
            {
                kfsetting->_timer_refresh_time = timertime;
            }
        }

    public:
        // 重置刷新时间表
        KFHashMap< uint32, uint32, KFTaskRefreshTimeData > _reset_data_list;
    };

}

#endif