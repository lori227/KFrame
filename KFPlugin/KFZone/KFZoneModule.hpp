#ifndef __KF_ZONE_MODULE_H__
#define __KF_ZONE_MODULE_H__

/************************************************************************
//    @Module			:    游戏分区模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFZoneInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"

namespace KFrame
{
    class KFZoneModule : public KFZoneInterface
    {
    public:
        KFZoneModule() = default;
        ~KFZoneModule() = default;

        // 初始化
        virtual void InitModule();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 游戏分区id
        virtual const KFZone* GetZone();

        // 游戏分区信息
        virtual const KFZone* GetZone( uint32 zoneid );

        // 判断游戏分区id
        virtual bool IsServerSameZone( uint64 serverid );
        virtual bool IsPlayerSameZone( uint64 playerid );
    };
}

#endif