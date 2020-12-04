#ifndef __KF_LOG_SHARD_MODULE__
#define __KF_LOG_SHARD_MODULE__

#include "KFrame.h"
#include "KFLogShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFLogShardModule : public KFLogShardInterface
    {
    public:
        KFLogShardModule() = default;
        virtual ~KFLogShardModule() = default;

        void BeforeRun();
        void BeforeShut();

    protected:
        // 请求连接地址
        __KF_HTTP_FUNCTION__( HandleRequestLogAddressReq );

        // 远程日志
        __KF_MESSAGE_FUNCTION__( HandleRemoteLogToServerReq, KFMsg::S2SRemoteLogToServerReq );
    };

}

#endif //!__KF_LOG_SHARD_MODULE__