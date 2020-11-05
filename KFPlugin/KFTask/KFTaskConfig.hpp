#ifndef __KF_TASK_CONFIG_H__
#define __KF_TASK_CONFIG_H__

#include "KFConfig.h"
#include "KFCore/KFCondition.h"
#include "KFXmlReader/KFExecuteData.h"
#include "KFXmlReader/KFElementConfig.h"

namespace KFrame
{
    enum KFTaskEnum
    {
        CompleteRequest = 1,	// 玩家请求交付
        ComplelteAuto = 2,		// 自动交付
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFTaskSetting : public KFIntSetting
    {
    public:
        // 任务类型
        uint32 _type = 0u;

        // 完成方式( 1=玩家请求交付 2=ComplelteAuto )
        uint32 _complete_type = 0u;

        // 前置条件
        uint32 _pre_condition_type = 0u;
        UInt32Vector _pre_condition;

        // 完成条件
        uint32 _complete_condition_type = 0u;
        UInt32Vector _complete_condition;

        // 输出( 掉落等 )
        UInt32Vector _execute_list;
    public:
        // 是否是自动提交类型
        bool IsAutoFinish() const
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

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTaskSetting* kfsetting );
    };
}

#endif