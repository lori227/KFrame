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
#include "KFDataConfig.hpp"

namespace KFrame
{
    class KFKernelModule : public KFKernelInterface
    {
    public:
        KFKernelModule() = default;
        ~KFKernelModule() = default;

        virtual void Run();
        virtual void AfterRun();
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 查找组件
        virtual KFComponent* FindComponent( const std::string& dataname );

        // 查找实体
        virtual KFEntity* FindEntity( const std::string& dataname, uint64 key, const char* function, uint32 line );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 创建数据
        virtual KFData* CreateData( const KFDataSetting* datasetting );

        // 释放数据
        virtual void DestroyData( KFData* kfdata );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 反序列化
        virtual bool ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto );

        // 序列化
        virtual KFMsg::PBObject* Serialize( KFData* kfdata );

        // 序列化存储到数据库
        virtual KFMsg::PBObject* SerializeToData( KFData* kfdata );

        // 序列化到观察者
        virtual KFMsg::PBObject* SerializeToView( KFData* kfdata );

        // 序列化到客户端
        virtual KFMsg::PBObject* SerializeToClient( KFData* kfdata );

        // 序列化到客户端( 优化上线的数据量, 登录时才使用 )
        virtual KFMsg::PBObject* SerializeToOnline( KFEntity* kfentity, uint32 delaytime = 0u );
        /////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 初始化
        void CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto );

        // 序列号
        KFMsg::PBObject* SerializeObject( KFData* kfdata, uint32 datamask, bool online, uint32 delaytime );

        // 保存
        void SaveToEntity( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online, uint32 delaytime );
        void SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask, bool online = false, uint32 delaytime = 0u );

    private:
        // 组件列表
        KFHashMap< std::string, KFComponentEx > _kf_component;
    };
}

#endif