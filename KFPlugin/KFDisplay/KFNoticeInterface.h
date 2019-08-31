#ifndef __KF_NOTICE_INTERFACE_H__
#define __KF_NOTICE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFNoticeInterface : public KFModule
    {
    public:
        // 发送小红点信息
        virtual void SendToPlayer( KFEntity* player, uint32 type, uint32 count = 1u ) = 0;
        virtual void SendToPlayer( KFData* kfbasic, uint32 type, uint32 count = 1u ) = 0;

        virtual void SendToPlayer( const Route& route, uint32 type, uint32 count = 1u ) = 0;
        virtual void SendToPlayer( uint64 serverid, uint64 playerid, uint32 type, uint32 count = 1u ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_notice, KFNoticeInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif