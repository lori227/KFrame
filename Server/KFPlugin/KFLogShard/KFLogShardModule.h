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

        virtual void Log( const ELogCategory& category, const int level, const uint32 zone_id, const std::string& app_name, const std::string& app_type, const uint32 app_id, const std::string& log_info );

    protected:
        void CreateLogger( const std::string& zone_name, const std::string& app_name, const std::string& app_type, const uint32 app_id, const std::string& category );
        const std::shared_ptr<spdlog::logger>& GetLogger( const ELogCategory& category, const uint32 zone_id, const std::string& app_name, const std::string& app_type, const uint32 app_id );

    protected:
        __KF_MESSAGE_FUNCTION__( HandleLogReq );

    private:
        std::unordered_map< std::string, std::shared_ptr<spdlog::logger> > _loggers;
    };

}

#endif //!__KF_LOG_SHARD_MODULE__