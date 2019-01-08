#ifndef __KF_CLUSTER_CLIENT_INTERFACE_H__
#define __KF_CLUSTER_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( uint64 ) > KFClusterConnectionFunction;
    class KFClusterClientInterface : public KFModule
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册回调
        template< class T >
        void RegisterConnectionFunction( T* object, void ( T::*handle )( uint64 ) )
        {
            KFClusterConnectionFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddConnectionFunction( typid( T ).name(), function );
        }

        // 卸载回调
        template< class T >
        void UnRegisterConnectionFunction( T* object )
        {
            RemoveConnectionFunction( typid( T ).name() );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 发送消息
        virtual bool SendToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;

    protected:
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function ) = 0;
        virtual void RemoveConnectionFunction( const std::string& name ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster, KFClusterClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif