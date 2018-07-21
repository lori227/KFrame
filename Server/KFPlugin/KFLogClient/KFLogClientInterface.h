/* ------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/07/10
------------------------------------*/
#ifndef __KF_LOG_CLIENT_INTERFACE__
#define __KF_LOG_CLIENT_INTERFACE__

#include "KFrame.h"
#include "spdlog/spdlog.h"

namespace KFrame
{

#define KF_REMOTE_LOG_TRACE(category, log_info)		_kf_log_client->LogRemote( spdlog::level::trace, category, log_info );
#define KF_REMOTE_LOG_DEBUG(category, log_info)		_kf_log_client->LogRemote( spdlog::level::debug, category, log_info );
#define KF_REMOTE_LOG_INFO(category, log_info)		_kf_log_client->LogRemote( spdlog::level::info, category, log_info );
#define KF_REMOTE_LOG_WARN(category, log_info)		_kf_log_client->LogRemote( spdlog::level::warn, category, log_info );
#define KF_REMOTE_LOG_ERROR(category, log_info)		_kf_log_client->LogRemote( spdlog::level::err, category, log_info );
#define KF_REMOTE_LOG_CRITICAL(category, log_info)	_kf_log_client->LogRemote( spdlog::level::critical, category, log_info );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFLogClientInterface : public KFModule
    {
    public:
        virtual void LogRemote( const int log_level, const int log_category, const std::string& log_info ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_log_client, KFLogClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //!__KF_LOG_CLIENT_INTERFACE__