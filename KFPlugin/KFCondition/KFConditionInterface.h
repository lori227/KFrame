#ifndef __KF_CONDITION_INTERFACE_H__
#define __KF_CONDITION_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFConditionInterface : public KFModule
    {
    public:
        // 判断静态配置条件
        virtual bool CheckStaticCondition( DataPtr object_data, const StaticConditionsPtr& static_conditions ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( EntityPtr entity, DataPtr condition_object, const DynamicConditionGroupPtr& condition_group ) = 0;

        // 初始化条件
        virtual bool InitCondition( EntityPtr entity, DataPtr condition_object, bool update ) = 0;

        // 直接更新条件
        virtual bool UpdateCondition( EntityPtr entity, DataPtr condition_object, uint32 condition_id, uint32 operate, uint32 condition_value ) = 0;

        // 判断动态计数条件
        virtual bool CheckCondition( EntityPtr entity, DataPtr condition_object ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( EntityPtr entity, DataPtr condition_object, DataPtr data ) = 0;

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( EntityPtr entity, DataPtr condition_object, DataPtr data ) = 0;

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( EntityPtr entity, DataPtr condition_object, DataPtr data, uint32 operate, uint64 value, uint64 now_value ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_condition, KFConditionInterface );
    //////////////////////////////////////////////////////////////////////////////////////

}



#endif