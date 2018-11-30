#ifndef __KF_WORKER_INTERFACE_H__
#define __KF_WORKER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
#define __REGISTER_WORKER_MESSAGE__( msgid, function ) _kf_worker->RegisterFunction( msgid, this, function )
#define __UNREGISTER_WORKER_MESSAGE__( msgid ) _kf_worker->UnRegisterFunction( msgid )

    class KFRedisDriver;
    class KFWorkerInterface : public KFModule
    {
    public:
        // 添加消息函数
        template<typename T>
        void RegisterFunction( uint32 msgid, T* object, void ( T::*handle )( const KFId& kfid, const char* data, uint32 length ) )
        {
            KFMessageFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddFunction( msgid, function );
        }

        // 取消注册
        virtual void UnRegisterFunction( uint32 msgid ) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息
        virtual void SendToClient( const KFId& kfid, uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual void SendToClient( const KFId& kfid, uint64 serverid, uint32 msgid, google::protobuf::Message* message ) = 0;

    protected:

        // 添加消息函数
        virtual void AddFunction( uint32 msgid, KFMessageFunction& function ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_worker, KFWorkerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
}



#endif