#ifndef __KF_DISPLAY_MOUDLE_H__
#define __KF_DISPLAY_MOUDLE_H__

/************************************************************************
//    @Module			:    结果提示系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-24
************************************************************************/

#include "KFDisplayInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFDisplayModule : public KFDisplayInterface
    {
    public:
        KFDisplayModule() = default;
        ~KFDisplayModule() = default;

        ////////////////////////////////////////////////////////////////////////////////
    private:
        // Gate/Proxy ==> Client
        virtual void SendToClient( uint64 playerid, uint32 result, StringList& params );

        // Game ==> Client
        virtual void SendToClient( KFEntity* player, uint32 result, StringList& params );

        // Route ==> Player
        virtual void SendToPlayer( KFData* kfbasic, uint32 result, StringList& params );
        virtual void SendToPlayer( const Route& route, uint32 result, StringList& params );
        virtual void SendToPlayer( uint64 serverid, uint64 playerid, uint32 result, StringList& params );

        // Game ==> Client
        virtual void SendToGroup( KFEntity* player, uint32 result, StringList& params );

    };
}



#endif