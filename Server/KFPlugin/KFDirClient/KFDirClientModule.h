#ifndef __KF_DIR_CLIENT_MODULE_H__
#define __KF_DIR_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    目录客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-20-20
************************************************************************/

#include "KFrame.h"
#include "KFDirClientInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFGate/KFGateInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFDirClientModule : public KFDirClientInterface
    {
    public:
        KFDirClientModule();
        ~KFDirClientModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 更新在线玩家数量
        __KF_TIMER_FUNCTION__( OnTimerUpdateOnlineToDir );

        // 连接到DirProxy
        void OnConnectionDirCluster( uint64 serverid );
    };
}



#endif