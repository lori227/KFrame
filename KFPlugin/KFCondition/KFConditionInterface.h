#ifndef __KF_CONDITION_INTERFACE_H__
#define __KF_CONDITION_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFConditionInterface : public KFModule
    {
    public:
        // 判断静态配置条件
        virtual bool CheckStaticCondition( DataPtr object_data, const StaticConditionsPtr& kfconditions ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( EntityPtr kfentity, DataPtr kfconditionobject, const DynamicConditionGroupPtr& conditiongroup ) = 0;

        // 初始化条件
        virtual bool InitCondition( EntityPtr kfentity, DataPtr kfconditionobject, bool update ) = 0;

        // 直接更新条件
        virtual bool UpdateCondition( EntityPtr kfentity, DataPtr kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue ) = 0;

        // 判断动态计数条件
        virtual bool CheckCondition( EntityPtr kfentity, DataPtr kfconditionobject ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( EntityPtr kfentity, DataPtr kfconditionobject, DataPtr kfdata ) = 0;

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( EntityPtr kfentity, DataPtr kfconditionobject, DataPtr kfdata ) = 0;

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( EntityPtr kfentity, DataPtr kfconditionobject, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_condition, KFConditionInterface );
    //////////////////////////////////////////////////////////////////////////////////////

}



#endif