#ifndef __KF_CHAT_MODULE_H__
#define __KF_CHAT_MODULE_H__

/************************************************************************
//    @Moudle			:    江湖聊天功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-03-08
************************************************************************/

#include "KFrame.h"
#include "KFChatInterface.h"
#include "KFChatPlayerEX.h"
#include "KFGame/KFGameInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRelationClient/KFRelationClientInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFChatModule : public KFChatInterface
    {
    public:
        KFChatModule();
        ~KFChatModule();

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
        //江湖聊天室
        KFMap < uint32, uint32, KFChatPlayerEX > _chat_player_list;
    };
}

#endif