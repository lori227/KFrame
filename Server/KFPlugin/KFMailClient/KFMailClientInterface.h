#ifndef __KF_MAIL_CLIENT_INTERFACE_H__
#define __KF_MAIL_CLIENT_INTERFACE_H__

#include "KFKernel/KFEntity.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMailClientInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////
        // 发送邮件
        virtual bool SendMail( KFEntity* player, uint32 mailconfigid, const KFAgents* kfagents ) = 0;

        // 发送邮件到对方
        virtual bool SendMail( KFEntity* player, uint64 toplayerid, uint32 mailconfigid, const KFAgents* kfagents ) = 0;

        // 发送邮件到对方
        virtual bool SendMail( KFEntity* player, uint64 toplayerid, uint32 mailconfigid, const std::string& extend ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mail, KFMailClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif