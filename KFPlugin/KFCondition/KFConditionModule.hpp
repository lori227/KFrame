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
#include "KFDisplay/KFDisplayInterface.h"
#include "KFConfig/KFConditionConfig.hpp"

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
        virtual bool CheckStaticCondition( DataPtr object_data, const StaticConditionsPtr& kfconditions );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( KFEntity* kfentity, DataPtr kfconditionobject, const DynamicConditionGroupPtr& conditiongroup );

        // 初始化条件
        virtual bool InitCondition( KFEntity* kfentity, DataPtr kfconditionobject, bool update );

        // 直接更新条件
        virtual bool UpdateCondition( KFEntity* kfentity, DataPtr kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 判断动态计数条件
        virtual bool CheckCondition( KFEntity* kfentity, DataPtr kfconditionobject );

        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata );

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue );

    protected:
        // 判断属性条件
        bool CheckStaticCondition( DataPtr object_data, const KFStaticCondition* kfcondition );

        // 计算条件表达式
        uint32 CalcExpression( DataPtr object_data, const KFStaticConditionExpression* kfexpression );

        // 计算属性值
        uint32 CalcConditionData( DataPtr object_data, const KFStaticConditionAbstract* kfconditiondata );

        // 判断限制条件
        bool CheckStaticConditionLimit( DataPtr object_data, const KFConditionSetting* kfsetting );

        // 计算触发值
        bool CalcTriggerUpdateValue( const ConditionTrigger* trigger, uint64 operate, uint64& operate_value, uint64 nowvalue );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 初始化条件
        uint32 InitConditionData( KFEntity* kfentity, DataPtr kfcondition, bool update );
        // 初始化计算数量
        uint32 InitCalcRecordCount( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting );
        // 计算是否存在
        uint32 InitCalcRecordExist( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting );
        // 计算获得数值
        uint32 InitCalcGetValue( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting );
        // 初始化计算数值
        uint32 InitCalcRecordValue( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting );
        // 获得集合列表
        std::list< DataPtr >& FindRecordList( KFEntity* kfentity, const std::string& name );

        // 直接更新数值
        uint32 UpdateConditionData( KFEntity* kfentity, DataPtr kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 添加属性更新条件, 返回true, 条件全部完成
        uint32 UpdateAddConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        uint32 UpdateRemoveConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata );

        // 更新条件
        uint32 UpdateUpdateConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue );

        // 判断属性条件
        uint32 CheckConditionData( KFEntity* kfentity, DataPtr kfcondition );

        // 计算添加数值
        std::tuple<uint32, uint32> CalcAddConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata );

        // 计算删除数值
        std::tuple<uint32, uint32> CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata );

        // 计算更新数值
        std::tuple<uint32, uint32> CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue );

        // 保存纪录的keyid
        void SaveConditionDataUUid( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, const KFConditionDefineSetting* kfsetting );

        // 判断是否存在key
        bool CheckUUidInConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, const KFConditionDefineSetting* kfsetting );
    };
}

#endif