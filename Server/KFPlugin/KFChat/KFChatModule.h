#ifndef __KF_CHAT_MODULE_H__
#define __KF_CHAT_MODULE_H__

/************************************************************************
//    @Module			:    江湖聊天功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-03-08
************************************************************************/

#include "KFrame.h"
#include "KFChatInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFRelationClient/KFRelationClientInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    // 聊天室玩家
    class KFChatPlayer
    {
    public:
        // playerid
        uint32 _player_id{ 0 };

        // gateid
        uint32 _gate_id{ 0 };

        // 上次发言时间
        uint64 _last_chat_time{ 0 };
    };

    class KFChatModule : public KFChatInterface
    {
    public:
        KFChatModule() = default;
        ~KFChatModule() = default;

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        //处理加入江湖聊天室请求
        __KF_MESSAGE_FUNCTION__( HandleEnterChatReq );

        //处理发送江湖聊天消息请求
        __KF_MESSAGE_FUNCTION__( HandleSendChatReq );

        //处理离开江湖聊天室请求
        __KF_MESSAGE_FUNCTION__( HandleLeaveChatReq );

        // 聊天信息
        __KF_MESSAGE_FUNCTION__( HandleSendChatToServerReq );
        //////////////////////////////////////////////////////////////////////

        //处理客户端发送一对一聊天请求
        __KF_MESSAGE_FUNCTION__( HandleSendOneByOneChatReq );

        //处理客户端发送大厅组队聊天请求
        __KF_MESSAGE_FUNCTION__( HandleSendGroupChatReq );

        //////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////
        //玩家上线回调
        void OnEnterEnterChat( KFEntity* player );

        //玩家离线回调
        void OnLeaveLeaveChat( KFEntity* player );

    private:
        // 江湖聊天室
        KFMap < uint32, uint32, KFChatPlayer > _chat_player_list;
    };
}

#endif