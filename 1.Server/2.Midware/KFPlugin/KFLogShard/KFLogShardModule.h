#ifndef __KF_LOG_SHARD_MODULE__
#define __KF_LOG_SHARD_MODULE__

#include "KFrame.h"
#include "KFLogShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFLogShardModule : public KFLogShardInterface
    {
    public:
        KFLogShardModule() = default;
        virtual ~KFLogShardModule();

        void BeforeRun();
        void BeforeShut();

    protected:

        KFSpdLog* FindRemoteLog( uint64 appid, const std::string& appname, const std::string& apptype, const std::string& strappid );

    protected:

        // 请求连接地址
        __KF_HTTP_FUNCTION__( HandleRequestLogAddressReq );

        // 远程日志
        __KF_MESSAGE_FUNCTION__( HandleRemoteLogToServerReq );
    private:
        // 远程日志
        std::unordered_map< uint64, KFSpdLog* > _log_list;
    };

}

#endif //!__KF_LOG_SHARD_MODULE__