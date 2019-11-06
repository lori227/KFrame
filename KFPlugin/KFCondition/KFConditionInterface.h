#ifndef __KF_CONDITION_INTERFACE_H__
#define __KF_CONDITION_INTERFACE_H__

#include "KFrame.h"
#include "KFCore/KFCondition.h"

namespace KFrame
{
    class KFConditionInterface : public KFModule
    {
    public:
        // 判断属性条件
        virtual bool CheckCondition( KFEntity* kfentity, const KFConditions* kfconditions ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( KFData* kfconditiondata, const VectorUInt32& conditionlist ) = 0;

        // 初始化条件
        virtual bool InitCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, bool update ) = 0;

        // 直接更新条件
        virtual bool UpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 conditionid, uint32 operate, uint32 conditionvalue ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata ) = 0;

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata ) = 0;

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_condition, KFConditionInterface );
    //////////////////////////////////////////////////////////////////////////////////////

}



#endif