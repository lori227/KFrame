#ifndef __KF_NOTICE_INTERFACE_H__
#define __KF_NOTICE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFNoticeInterface : public KFModule
    {
    public:
        // 发送小红点信息
        virtual void SendToPlayer( EntityPtr player, uint32 type, uint32 count = 1u ) = 0;
        virtual void SendToPlayer( DataPtr basic_data, uint32 type, uint32 count = 1u ) = 0;

        virtual void SendToPlayer( const Route& route, uint32 type, uint32 count = 1u ) = 0;
        virtual void SendToPlayer( uint64 server_id, uint64 player_id, uint32 type, uint32 count = 1u ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_notice, KFNoticeInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif