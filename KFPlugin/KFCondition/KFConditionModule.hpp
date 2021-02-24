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
        virtual bool CheckStaticCondition( DataPtr object_data, const StaticConditionsPtr& static_conditions );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 初始化条件
        virtual void AddCondition( EntityPtr entity, DataPtr condition_object, const DynamicConditionGroupPtr& condition_group );

        // 初始化条件
        virtual bool InitCondition( EntityPtr entity, DataPtr condition_object, bool update );

        // 直接更新条件
        virtual bool UpdateCondition( EntityPtr entity, DataPtr condition_object, uint32 condition_id, uint32 operate, uint32 condition_value );

        // 判断动态计数条件
        virtual bool CheckCondition( EntityPtr entity, DataPtr condition_object );

        // 添加属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateAddCondition( EntityPtr entity, DataPtr condition_object, DataPtr data );

        // 删除属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateRemoveCondition( EntityPtr entity, DataPtr condition_object, DataPtr data );

        // 更新属性更新条件, 返回true, 条件全部完成
        virtual bool UpdateUpdateCondition( EntityPtr entity, DataPtr condition_object, DataPtr data, uint32 operate, uint64 value, uint64 now_value );

    protected:
        // 判断属性条件
        bool CheckStaticCondition( DataPtr object_data, std::shared_ptr<const KFStaticCondition> static_condition );

        // 计算条件表达式
        uint32 CalcExpression( DataPtr object_data, std::shared_ptr<const KFStaticConditionExpression> static_expression );

        // 计算属性值
        uint32 CalcConditionData( DataPtr object_data, std::shared_ptr<const KFStaticConditionAbstract> condition_data );

        // 判断限制条件
        bool CheckStaticConditionLimit( DataPtr object_data, std::shared_ptr<const KFConditionSetting> setting );

        // 计算触发值
        bool CalcTriggerUpdateValue( const ConditionTrigger* trigger, uint64 operate, uint64& operate_value, uint64 now_value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 初始化条件
        uint32 InitConditionData( EntityPtr entity, DataPtr condition_object, bool update );
        // 初始化计算数量
        uint32 InitCalcRecordCount( EntityPtr entity, DataPtr condition_object, std::shared_ptr<const KFConditionSetting> setting );
        // 计算是否存在
        uint32 InitCalcRecordExist( EntityPtr entity, DataPtr condition_object, std::shared_ptr<const KFConditionSetting> setting );
        // 计算获得数值
        uint32 InitCalcGetValue( EntityPtr entity, DataPtr condition_object, std::shared_ptr<const KFConditionSetting> setting );
        // 初始化计算数值
        uint32 InitCalcRecordValue( EntityPtr entity, DataPtr condition_object, std::shared_ptr<const KFConditionSetting> setting );
        // 获得集合列表
        std::list<DataPtr>& FindRecordList( EntityPtr entity, const std::string& name );

        // 直接更新数值
        uint32 UpdateConditionData( EntityPtr entity, DataPtr condition_object, uint32 condition_id, uint32 operate, uint32 condition_value );

        // 添加属性更新条件, 返回true, 条件全部完成
        uint32 UpdateAddConditionData( EntityPtr entity, DataPtr condition_object, DataPtr data );

        // 删除属性更新条件, 返回true, 条件全部完成
        uint32 UpdateRemoveConditionData( EntityPtr entity, DataPtr condition_object, DataPtr data );

        // 更新条件
        uint32 UpdateUpdateConditionData( EntityPtr entity, DataPtr condition_object, DataPtr data, uint32 operate, uint64 value, uint64 now_value );

        // 判断属性条件
        uint32 CheckConditionData( EntityPtr entity, DataPtr condition_object );

        // 计算添加数值
        std::tuple<uint32, uint32> CalcAddConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data );

        // 计算删除数值
        std::tuple<uint32, uint32> CalcRemoveConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data );

        // 计算更新数值
        std::tuple<uint32, uint32> CalcUpdateConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data, uint32 operate, uint64 value, uint64 now_value );

        // 保存纪录的keyid
        void SaveConditionDataUUid( EntityPtr entity, DataPtr condition_object, DataPtr data, std::shared_ptr<const KFConditionDefineSetting> setting );

        // 判断是否存在key
        bool CheckUUidInConditionData( EntityPtr entity, DataPtr condition_object, DataPtr data, std::shared_ptr<const KFConditionDefineSetting> setting );
    };
}

#endif