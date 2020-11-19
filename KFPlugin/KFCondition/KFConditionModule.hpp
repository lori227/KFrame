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
#include "KFXmlReader/KFConditionConfig.hpp"

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
        virtual bool CheckStaticCondition( KFData* kfobject, const StaticConditionsPtr& kfconditions );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( KFEntity* kfentity, KFData* kfconditionobject, const DynamicConditionGroupPtr& conditiongroup );

        // 初始化条件
        virtual bool InitCondition( KFEntity* kfentity, KFData* kfconditionobject, bool update );

        // 直接更新条件
        virtual bool UpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 判断动态计数条件
        virtual bool CheckCondition( KFEntity* kfentity, KFData* kfconditionobject );

        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( KFEntity* kfentity, KFData* kfconditionobject, KFData* kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditionobject, KFData* kfdata );

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );

    protected:
        // 判断属性条件
        bool CheckStaticCondition( KFData* kfobject, const KFStaticCondition* kfcondition );

        // 计算条件表达式
        uint32 CalcExpression( KFData* kfobject, const KFStaticConditionExpression* kfexpression );

        // 计算属性值
        uint32 CalcConditionData( KFData* kfobject, const KFStaticConditionAbstract* kfconditiondata );

        // 判断限制条件
        bool CheckStaticConditionLimit( KFData* kfobject, const KFConditionSetting* kfsetting );

        // 计算触发值
        bool CalcTriggerUpdateValue( const ConditionTrigger* trigger, uint64 operate, uint64& operatevalue, uint64 nowvalue );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 初始化条件
        uint32 InitConditionData( KFEntity* kfentity, KFData* kfcondition, bool update );
        // 初始化计算数量
        uint32 InitCalcRecordCount( KFEntity* kfentity, KFData* kfcondition, const KFConditionSetting* kfsetting );
        // 计算是否存在
        uint32 InitCalcRecordExist( KFEntity* kfentity, KFData* kfcondition, const KFConditionSetting* kfsetting );
        // 计算获得数值
        uint32 InitCalcGetValue( KFEntity* kfentity, KFData* kfcondition, const KFConditionSetting* kfsetting );
        // 初始化计算数值
        uint32 InitCalcRecordValue( KFEntity* kfentity, KFData* kfcondition, const KFConditionSetting* kfsetting );
        // 获得集合列表
        std::list< KFData* >& FindRecordList( KFEntity* kfentity, const std::string& name );

        // 直接更新数值
        uint32 UpdateConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue );

        // 添加属性更新条件, 返回true, 条件全部完成
        uint32 UpdateAddConditionData( KFEntity* kfentity, KFData* kfcondition, KFData* kfdata );

        // 删除属性更新条件, 返回true, 条件全部完成
        uint32 UpdateRemoveConditionData( KFEntity* kfentity, KFData* kfcondition, KFData* kfdata );

        // 更新条件
        uint32 UpdateUpdateConditionData( KFEntity* kfentity, KFData* kfcondition, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );

        // 判断属性条件
        uint32 CheckConditionData( KFEntity* kfentity, KFData* kfcondition );

        // 计算添加数值
        std::tuple<uint32, uint32> CalcAddConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, KFData* kfdata );

        // 计算删除数值
        std::tuple<uint32, uint32> CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, KFData* kfdata );

        // 计算更新数值
        std::tuple<uint32, uint32> CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );

        // 保存纪录的keyid
        void SaveConditionDataUUid( KFEntity* kfentity, KFData* kfcondition, KFData* kfdata, const KFConditionDefineSetting* kfsetting );

        // 判断是否存在key
        bool CheckUUidInConditionData( KFEntity* kfentity, KFData* kfcondition, KFData* kfdata, const KFConditionDefineSetting* kfsetting );
    };
}

#endif