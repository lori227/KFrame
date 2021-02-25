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
        virtual bool SendMail( uint32 config_id, const KFElements* elements = nullptr ) = 0;

        // 系统给某人发送邮件
        virtual bool SendMail( uint64 recv_id, uint32 config_id, const KFElements* elements = nullptr ) = 0;

        // 某人给某人发送邮件
        virtual bool SendMail( EntityPtr player, uint64 recv_id, uint32 config_id, const KFElements* elements = nullptr ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mail, KFMailClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif