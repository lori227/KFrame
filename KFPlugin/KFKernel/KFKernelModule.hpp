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
        virtual std::shared_ptr<KFComponent> FindComponent( const std::string& data_name );

        // 查找实体
        virtual EntityPtr FindEntity( const std::string& data_name, uint64 key, const char* function, uint32 line );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 反序列化
        virtual bool ParseFromMessage( DataPtr object_data, const KFMsg::PBObject* message );

        // 序列化
        virtual KFMsg::PBObject* Serialize( DataPtr object_data );

        // 序列化存储到数据库
        virtual KFMsg::PBObject* SerializeToData( DataPtr object_data );

        // 序列化到观察者
        virtual KFMsg::PBObject* SerializeToView( DataPtr object_data );

        // 序列化到客户端
        virtual KFMsg::PBObject* SerializeToClient( DataPtr object_data );

        // 序列化到客户端( 优化上线的数据量, 登录时才使用 )
        virtual KFMsg::PBObject* SerializeToOnline( EntityPtr entity, uint32 delay_time = 0u );
        /////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 初始化
        void CopyFromObject( DataPtr object_data, const KFMsg::PBObject* message );

        // 序列号
        KFMsg::PBObject* SerializeObject( DataPtr object_data, uint32 data_mask, bool online, uint32 delay_time );

        // 保存
        void SaveToEntity( DataPtr object_data, KFMsg::PBObject* message, uint32 data_mask, bool online, uint32 delay_time );
        void SaveToObject( DataPtr object_data, KFMsg::PBObject* message, uint32 data_mask, bool online = false, uint32 delay_time = 0u );

    private:
        // 组件列表
        KFHashMap<std::string, KFComponentEx> _component;
    };
}

#endif