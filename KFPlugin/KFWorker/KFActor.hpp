#ifndef __KF_ACTOR_H__
#define __KF_ACTOR_H__

#include "KFrame.h"
#include "KFWorkerMessage.hpp"
#include "KFRedis/KFRedisInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
    class KFWorkerModule;
    /////////////////////////////////////////////////////////////////////////
    class KFActor
    {
    public:
        KFActor();
        ~KFActor();

        // 初始化
        void InitActor( KFWorkerModule* kfmodule, uint32 maxreqcount, uint32 maxackcount );

        // 逻辑
        void RunActor();

        // 停止
        void StopActor();

        // 是否忙
        bool IsBusy();

        // 添加请求消息
        bool PushReqMessage( const Route& route, uint32 msgid, const char* data, uint32 length );

        // 添加响应消息
        bool PushAckMessage( const Route& route, uint32 msgid, google::protobuf::Message* message );
        bool PushAckMessage( uint64 server_id, uint32 msgid, google::protobuf::Message* message );

    protected:
        // 执行线程逻辑
        void RunActorThread();

    private:
        // 工作模块
        KFWorkerModule* _kf_worker_moduler;

        // 线程状态
        std::atomic<bool> _actor_thread_run;

        // 暂存的Guid
        Route _route;

        // 需要处理的请求消息
        KFQueue< KFWorkerMessage > _req_message_queue;

        // 已经处理好的消息
        KFQueue< KFWorkerMessage > _ack_message_queue;
    };
}



#endif