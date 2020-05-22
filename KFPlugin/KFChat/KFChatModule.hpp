#ifndef __KF_CHAT_MOUDLE_H__
#define __KF_CHAT_MOUDLE_H__

/************************************************************************
//    @Moudle			:    聊天系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-5-20
************************************************************************/
#include "KFrame.h"
#include "KFChatInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFFilter/KFFilterInterface.h"

namespace KFrame
{
    class KFChatModule : public KFChatInterface
    {
    public:
        KFChatModule() = default;
        ~KFChatModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 好友私聊
        __KF_MESSAGE_FUNCTION__( HandleFriendChatReq );

        // 本服务器(进程)的公屏聊天
        __KF_MESSAGE_FUNCTION__( HandleServerChatReq );
    };
}



#endif