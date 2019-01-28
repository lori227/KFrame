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
#include "KFKernelInterface.h"
#include "KFComponentEx.hpp"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFKernelModule : public KFKernelInterface
    {
    public:
        KFKernelModule() = default;
        ~KFKernelModule() = default;

        virtual void InitModule();
        virtual void Run();
        virtual void AfterRun();
        virtual void ShutDown();
        /////////////////////////////////////////////////////////////////////////////////////////////
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
        /////////////////////////////////////////////////////////////////////////////////////////////

        // proto 序列化
        virtual bool ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto );

        virtual void SerializeToClient( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToData( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToView( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToOnline( KFData* kfdata, KFMsg::PBObject* proto );

    public:
        // 初始化
        void CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto );

        // 保存
        void SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask );

    private:
        KFMap< std::string, const std::string, KFComponentEx > _kf_component;

    };
}

#endif