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
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息( 只发送一次, 不管成功还是失败 )
        virtual bool SendToProxy( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;

        virtual bool SendToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发送消息( 失败重复发送, 一直到发送成功 )
        virtual bool RepeatToProxy( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool RepeatToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;

        virtual bool RepeatToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool RepeatToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册回调
        template< class T >
        void RegisterConnectionFunction( T* object, void ( T::*handle )( uint64 ) )
        {
            KFClusterConnectionFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddConnectionFunction( typeid( T ).name(), function );
        }

        // 卸载回调
        template< class T >
        void UnRegisterConnectionFunction( T* object )
        {
            RemoveConnectionFunction( typeid( T ).name() );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function ) = 0;
        virtual void RemoveConnectionFunction( const std::string& name ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_client, KFClusterClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif