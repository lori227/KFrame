#ifndef __KF_CONDITION_MODULE_H__
#define __KF_CONDITION_MODULE_H__

/************************************************************************
//    @Module			:    条件判断模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFConditionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFConditionConfig.hpp"

namespace KFrame
{
    class KFConditionModule : public KFConditionInterface
    {
    public:
        KFConditionModule() = default;
        ~KFConditionModule() = default;

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 判断属性条件
        virtual bool CheckCondition( KFEntity* kfentity, const KFConditions* kfconditions );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( KFData* kfconditiondata, const VectorUInt32& conditionlist );

        // 初始化条件
        virtual bool InitCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, bool update );

        // 直接更新条件
        virtual bool UpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata );

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );


    protected:
        // 判断属性条件
        bool CheckCondition( KFEntity* kfentity, const KFCondition* kfcondition );

        // 计算条件表达式
        uint32 CalcExpression( KFEntity* kfentity, const KFExpression* kfexpression );

        // 计算属性值
        uint32 CalcConditionData( KFEntity* kfentity, const KFConditionData* kfconditiondata );

        // 判断限制条件
        bool CheckConditionLimit( KFEntity* kfentity, KFData* kfdata, const KFConditionSetting* kfsetting );

        // 初始化条件
        uint32 InitCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, bool update );
        // 初始化计算数量
        uint32 InitCalcRecordCount( KFEntity* kfentity, const KFConditionSetting* kfsetting );
        // 计算是否存在
        uint32 InitCalcRecordExist( KFEntity* kfentity, const KFConditionSetting* kfsetting );
        // 计算获得数值
        uint32 InitCalcGetValue( KFEntity* kfentity, const KFConditionSetting* kfsetting );

        // 直接更新数值
        uint32 UpdateCondition( KFEntity* kfentity, KFData* kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 添加属性更新条件, 返回true, 条件全部完成
        uint32 UpdateAddCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        uint32 UpdateRemoveCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata );

        // 更新条件
        uint32 UpdateUpdateCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );


        // 计算添加数值
        std::tuple<uint32, uint32> CalcAddConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata );

        // 计算删除数值
        std::tuple<uint32, uint32> CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata );

        // 计算更新数值
        std::tuple<uint32, uint32> CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );
    };
}

#endif