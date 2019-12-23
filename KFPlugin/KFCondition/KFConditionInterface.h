#ifndef __KF_CONDITION_INTERFACE_H__
#define __KF_CONDITION_INTERFACE_H__

#include "KFrame.h"
#include "KFCore/KFCondition.h"

namespace KFrame
{
    class KFConditionInterface : public KFModule
    {
    public:
        // 判断静态配置条件
        virtual bool CheckCondition( KFEntity* kfentity, const KFConditions* kfconditions ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( KFEntity* kfentity, KFData* kfconditionobject, const UInt32Vector& conditionlist, uint32 conditiontype ) = 0;

        // 初始化条件
        virtual bool InitCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, bool update ) = 0;

        // 直接更新条件
        virtual bool UpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue ) = 0;

        // 判断动态计数条件
        virtual bool CheckCondition( KFEntity* kfentity, KFData* kfconditionobject ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata ) = 0;

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata ) = 0;

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_condition, KFConditionInterface );
    //////////////////////////////////////////////////////////////////////////////////////

}



#endif