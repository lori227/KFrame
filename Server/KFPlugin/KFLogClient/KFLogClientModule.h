/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/07/10
------------------------------------*/
#ifndef __KF_LOG_CLIENT_MODULE__
#define __KF_LOG_CLIENT_MODULE__

#include "KFLogClientInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFLogClientModule : public KFLogClientInterface
    {
    public :
        KFLogClientModule() = default;
        ~KFLogClientModule() = default;

        virtual void BeforeRun();
        virtual void BeforeShut();

        // 远程日志
        bool LogRemote( uint32 loglevel, uint32 category, const std::string& loginfo );

    protected:
        // 连接到LogProxy
        void OnConnectionLogCluster( uint32 serverid );
    };
}


#endif //!__KF_LOG_CLIENT_MODULE__