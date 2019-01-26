#ifndef __KF_CLUSTER_CLIENT_INTERFACE_H__
#define __KF_CLUSTER_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterClientInterface : public KFModule
    {
    public:
        // 是否在服务中
        virtual bool IsInService() = 0;

        // 发送消息
        virtual bool SendToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual bool SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册回调
        template< class T >
        void RegisterConnectionFunction( T* object, void ( T::*handle )( uint64 ) )
        {
            KFClusterConnectionFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddConnectionFunction( typeid( T ).name(), function );
        }
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function ) = 0;

        // 卸载回调
        template< class T >
        void UnRegisterConnectionFunction( T* object )
        {
            RemoveConnectionFunction( typeid( T ).name() );
        }
        virtual void RemoveConnectionFunction( const std::string& name ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_client, KFClusterClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif