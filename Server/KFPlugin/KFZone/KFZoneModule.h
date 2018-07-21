#ifndef __KF_ZONE_MODULE_H__
#define __KF_ZONE_MODULE_H__

/************************************************************************
//    @Moudle			:    游戏分区模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFZoneInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"

namespace KFrame
{
    class KFZoneModule : public KFZoneInterface
    {
    public:
        KFZoneModule();
        ~KFZoneModule();

        // 初始化
        virtual void InitModule();

        virtual void AfterLoad();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 游戏分区id
        virtual const KFZone* GetZone() const;

        // 判断游戏分区id
        virtual bool IsServerSameZone( uint32 serverid );
        virtual bool IsPlayerSameZone( uint32 playerid );
    };
}

#endif