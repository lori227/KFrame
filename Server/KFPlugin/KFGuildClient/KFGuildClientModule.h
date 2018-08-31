#ifndef __KF_GUILD_CLIENT_MODULE_H__
#define __KF_GUILD_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    帮派数据模块
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
#include "KFDisplay/KFDisplayInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMailClient/KFMailClientInterface.h"

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

        virtual bool SendMessageToGuild( uint32 msgid, ::google::protobuf::Message* message );

        virtual void UpdateMemberBasic( uint32 playerid, uint64 guildid, MapString& basics );


    protected:
        // 处理创建帮派请求
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildReq );

        // 处理创建帮派反馈
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildAck );

        // 处理邀请玩家邀请加入帮派
        __KF_MESSAGE_FUNCTION__( HnadleInviteGuildReq );

        // 处理邀请玩家邀请加入帮派反馈
        __KF_MESSAGE_FUNCTION__( HnadleInviteGuildAck );

        // 处理玩家申请加入帮派
        __KF_MESSAGE_FUNCTION__( HandleApplyGuildReq );

        // 处理玩家帮派变化
        __KF_MESSAGE_FUNCTION__( HandlePlayerGuildChangeReq );
    };
}

#endif