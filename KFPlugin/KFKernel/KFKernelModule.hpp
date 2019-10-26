#ifndef __KF_KERNEL_MODULE_H__
#define __KF_KERNEL_MODULE_H__

/************************************************************************
//    @Module			:    游戏核心数据
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-12
************************************************************************/

#include "KFrame.h"
#include "KFComponentEx.hpp"
#include "KFKernelInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFZConfig/KFDataConfig.hpp"
#include "KFZConfig/KFElementConfig.hpp"
#include "KFZConfig/KFOptionConfig.hpp"


namespace KFrame
{
    class KFKernelModule : public KFKernelInterface
    {
    public:
        KFKernelModule();
        ~KFKernelModule();

        virtual void Run();
        virtual void AfterRun();
        /////////////////////////////////////////////////////////////////////////////////////////////
        static KFKernelModule* Instance();
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 查找组件
        virtual KFComponent* FindComponent( const std::string& dataname );

        // 查找实体
        virtual KFEntity* FindEntity( const std::string& dataname, uint64 key, const char* function, uint32 line );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 创建数据
        virtual KFData* CreateObject( const std::string& dataname );
        virtual KFData* CreateObject( const KFDataSetting* datasetting );
        virtual KFData* CreateObject( const std::string& classname, const std::string& dataname );
        virtual KFData* CreateObject( const KFDataSetting* datasetting, const KFMsg::PBObject* proto );

        // 释放数据
        virtual void ReleaseObject( KFData* kfdata );

        // 初始化数组( kfarray )
        virtual void InitArray( KFData* kfarray, uint32 size );

        // 添加数组元素( kfarray )
        virtual KFData* AddArray( KFData* kfarray );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 反序列化
        virtual bool ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto );

        // 序列化
        virtual KFMsg::PBObject* SerializeToClient( KFData* kfdata );
        virtual KFMsg::PBObject* SerializeToData( KFData* kfdata );
        virtual KFMsg::PBObject* SerializeToView( KFData* kfdata );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 判断属性条件
        virtual bool CheckCondition( KFEntity* kfentity, const KFConditions* kfconditions );
    public:
        // 初始化
        void CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto );

        // 保存
        void SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask );

        // 判断属性条件
        bool CheckCondition( KFEntity* kfentity, const KFCondition* kfcondition );

        // 计算条件表达式
        uint32 CalcExpression( KFEntity* kfentity, const KFExpression* kfexpression );

        // 计算属性值
        uint32 CalcConditionData( KFEntity* kfentity, const KFConditionData* kfconditiondata );

    private:
        // kernel
        static KFKernelModule* _kernel_module;

        // 组件列表
        KFHashMap< std::string, const std::string, KFComponentEx > _kf_component;
    };
}

#endif