#ifndef __KF_GUILD_CLIENT_MODULE_H__
#define __KF_GUILD_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    帮派数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFGuildClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFGuildClientModule : public KFGuildClientInterface
    {
    public:
        KFGuildClientModule();
        ~KFGuildClientModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Guild
        virtual bool SendMessageToGuild( uint64 guildid, uint32 msgid, ::google::protobuf::Message* message );

    };
}

#endif