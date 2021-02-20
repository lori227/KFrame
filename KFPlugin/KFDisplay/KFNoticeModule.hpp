#ifndef __KF_NOTICE_MOUDLE_H__
#define __KF_NOTICE_MOUDLE_H__

/************************************************************************
//    @Module			:    小红点通知模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-8-23
************************************************************************/

#include "KFNoticeInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFNoticeModule : public KFNoticeInterface
    {
    public:
        KFNoticeModule() = default;
        ~KFNoticeModule() = default;

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        virtual void SendToPlayer( KFEntity* player, uint32 type, uint32 count = 1u );
        virtual void SendToPlayer( DataPtr kfbasic, uint32 type, uint32 count = 1u );

        virtual void SendToPlayer( const Route& route, uint32 type, uint32 count = 1u );
        virtual void SendToPlayer( uint64 server_id, uint64 playerid, uint32 type, uint32 count = 1u );


    };
}



#endif