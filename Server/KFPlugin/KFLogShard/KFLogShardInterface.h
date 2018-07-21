/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_SHARD_INTERFACE__
#define __KF_LOG_SHARD_INTERFACE__

#include "KFrame.h"

namespace KFrame
{

    class KFLogShardInterface : public KFModule
    {
    public:
        virtual void Log( const ELogCategory& category, const int level, const uint32 zone_id, const std::string& app_name, const std::string& app_type, const uint32 app_id, const std::string& log_info ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_log_shard, KFLogShardInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //!__KF_LOG_SHARD_INTERFACE__