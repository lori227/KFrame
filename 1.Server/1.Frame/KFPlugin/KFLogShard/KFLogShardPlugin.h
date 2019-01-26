/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/07/10
------------------------------------*/
#ifndef __KF_LOG_SHARD_PLUGIN__
#define __KF_LOG_SHARD_PLUGIN__

#include "KFrame.h"

namespace KFrame
{

    class KFLogShardPlugin : public KFPlugin
    {
    public:
        virtual void Install();
        virtual void UnInstall();
        virtual void LoadModule();
    };

}


#endif //!__KF_LOG_SHARD_PLUGIN__