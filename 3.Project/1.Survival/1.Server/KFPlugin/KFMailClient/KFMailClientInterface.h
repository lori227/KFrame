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
        // 发送全局邮件
        virtual bool SendMail( uint32 configid, const KFElements* kfelements = nullptr ) = 0;

        // 系统给某人发送邮件
        virtual bool SendMail( uint64 recvid, uint32 configid, const KFElements* kfelements = nullptr ) = 0;

        // 某人给某人发送邮件
        virtual bool SendMail( KFEntity* player, uint64 recvid, uint32 configid, const KFElements* kfelements = nullptr ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mail, KFMailClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif