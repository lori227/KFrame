/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_SHARD_MODULE__
#define __KF_LOG_SHARD_MODULE__

#include "KFrame.h"
#include "KFLogShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "spdlog/spdlog.h"

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

        void Log( uint32 level, uint32 category, uint32 zoneid, const std::string& appname, const std::string& apptype, uint32 appid, const std::string& loginfo );

        void CreateLogger( const std::string& zonename, const std::string& appname, const std::string& apptype, uint32 appid, const std::string& category );
        const std::shared_ptr<spdlog::logger>& GetLogger( uint32 category, uint32 zoneid, const std::string& appname, const std::string& apptype, uint32 appid );

    protected:
        __KF_MESSAGE_FUNCTION__( HandleRemoteLogReq );

    private:
        std::unordered_map< std::string, std::shared_ptr<spdlog::logger> > _loggers;
    };

}

#endif //!__KF_LOG_SHARD_MODULE__