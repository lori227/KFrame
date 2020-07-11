#ifndef __KF_TASK_CONFIG_H__
#define __KF_TASK_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFCondition.h"
#include "KFZConfig/KFExecuteData.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    enum KFTaskEnum
    {
        CompleteRequest = 0,	// 玩家请求交付
        ComplelteAuto = 1,		// 自动交付
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFTaskSetting : public KFIntSetting
    {
    public:
        // 任务类型
        uint32 _type = 0u;

        // 任务区域
        uint32 _area = 1u;

        // 激活状态
        uint32 _active_status = 0u;

        // 完成方式
        uint32 _complete_type = 0u;

        // 前置条件
        uint32 _pre_condition_type = 0u;
        UInt32Vector _pre_condition;

        // 地点条件
        KFConditions _place_condition;

        // 完成条件
        uint32 _complete_condition_type = 0u;
        UInt32Vector _complete_condition;

        // 输出
        KFExecuteData _execute_data;
    public:
        // 是否是自动提交类型
        bool IsAutoTask() const
        {
            return _complete_type == KFTaskEnum::ComplelteAuto;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFTaskConfig : public KFConfigT< KFTaskSetting >, public KFInstance< KFTaskConfig >
    {
    public:
        KFTaskConfig()
        {
            _file_name = "task";
        }

        // 加载完成
        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTaskSetting* kfsetting );
    };
}

#endif