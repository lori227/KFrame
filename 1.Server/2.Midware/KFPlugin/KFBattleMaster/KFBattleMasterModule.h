#ifndef __KF_BATTLE_PROXY_MODULE_H__
#define __KF_BATTLE_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    战斗服务器管理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-10-26
************************************************************************/

#include "KFrame.h"
#include "KFBattleMasterInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFBattleMasterModule : public KFBattleMasterInterface
    {
    public:
        KFBattleMasterModule() = default;
        ~KFBattleMasterModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 分配战场id
        __KF_MESSAGE_FUNCTION__( HandleAllocBattleIdReq );

    protected:
        // 计算战场id
        uint64 AllocBattleId( const std::string& ip, uint32 port );

    private:
        KFRedisDriver* _battle_redis_driver{ nullptr };
    };
}



#endif