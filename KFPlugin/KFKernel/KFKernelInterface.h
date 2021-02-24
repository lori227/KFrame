#ifndef __KF_KERNEL_INTERFACE_H__
#define __KF_KERNEL_INTERFACE_H__

#include "KFEntity.h"
#include "KFComponent.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFKernelInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 查找组件
        virtual std::shared_ptr<KFComponent> FindComponent( const std::string& data_name ) = 0;

        // 查找实体
        virtual EntityPtr FindEntity( const std::string& data_name, uint64 key, const char* function, uint32 line ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 反序列化
        virtual bool ParseFromMessage( DataPtr object_data, const KFMsg::PBObject* proto ) = 0;

        // 序列化
        virtual KFMsg::PBObject* Serialize( DataPtr object_data ) = 0;

        // 序列化存储到数据库
        virtual KFMsg::PBObject* SerializeToData( DataPtr object_data ) = 0;

        // 序列化到观察者
        virtual KFMsg::PBObject* SerializeToView( DataPtr object_data ) = 0;

        // 序列化到客户端
        virtual KFMsg::PBObject* SerializeToClient( DataPtr object_data ) = 0;

        // 序列化到客户端( 优化上线的数据量, 登录时才使用 )
        virtual KFMsg::PBObject* SerializeToOnline( EntityPtr entity, uint32 delay_time = 0u ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_kernel, KFKernelInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif