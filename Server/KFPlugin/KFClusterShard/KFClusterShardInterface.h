#ifndef __KF_CLUSTER_SHARD_INTERFACE_H__
#define __KF_CLUSTER_SHARD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( const std::set< uint32 >& objectlist ) > KFAllocObjectFunction;

    class KFClusterShardInterface : public KFModule
    {
    public:
        // 发型消息到代理服务器
        virtual void SendToProxy( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual void SendToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发型消息到代理服务器
        virtual bool SendToProxy( uint32 handleid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToProxy( uint32 handleid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发型消息到代理服务器
        virtual bool SendToClient( const KFId& kfid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToClient( const KFId& kfid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发型消息到代理服务器
        virtual bool SendToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToClient( uint32 clientid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发型消息到代理服务器
        virtual bool SendToClient( uint32 proxyid, uint32 clientid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendToClient( uint32 proxyid, uint32 clientid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发送消息到玩家
        virtual bool SendToPlayer( uint32 clientid, uint32 playerid, uint32 msgid, google::protobuf::Message* message ) = 0;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        // 注册对象映射
        virtual void AddObjectToProxy( uint64 objectid ) = 0;
        virtual void AddObjectToProxy( uint32 proxyid, const std::set< uint64 >& objectlist ) = 0;

        // 删除对象映射
        virtual void RemoveObjectToProxy( uint64 objectid ) = 0;
        virtual void RemoveObjectToProxy( const std::set< uint64 >& objectlist ) = 0;

        // 分配Shard
        virtual void AllocObjectToMaster( const std::set< uint32 >& objectlist ) = 0;
        virtual const std::set< uint32 >& GetAllocObjectList() = 0;

        template< class T >
        void RegisterAllocObjectFunction( T* object, void ( T::*handle )( const std::set< uint32 >& objectlist ) )
        {
            KFAllocObjectFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddAllocObjectFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterAllocObjectFunction( T* object )
        {
            RemoveAllocObjectFunction( typeid( T ).name() );
        }

    protected:
        virtual void AddAllocObjectFunction( const std::string& module, KFAllocObjectFunction& function ) = 0;
        virtual void RemoveAllocObjectFunction( const std::string& module ) = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_shard, KFClusterShardInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_ALLOC_OBJECT_FUNCTION__( function ) \
    void function( const std::set< uint32 >& objectlist )

#define __REGISTER_ALLOC_OBJECT_FUNCTION__( function ) \
    _kf_cluster_shard->RegisterAllocObjectFunction( this, function )

#define __UNREGISTER_ALLOC_OBJECT_FUNCTION__() \
    _kf_cluster_shard->UnRegisterAllocObjectFunction( this )
}



#endif