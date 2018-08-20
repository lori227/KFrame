#ifndef __KF_ZONE_INTERFACE_H__
#define __KF_ZONE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 分区设置
    class KFZone
    {
    public:
        KFZone()
        {
            _id = 0;
            _channel = 0;
            _logic_id = 0;
        }

    public:
        // 游戏分区id
        uint32 _id;

        // 逻辑id
        uint32 _logic_id;

        // 游戏服类型
        uint32 _channel;

        // 游戏服名字
        std::string _name;

        // master服务器ip
        std::string _ip;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFZoneInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 游戏分区id
        virtual const KFZone* GetZone() const = 0;

        // 判断游戏分区id
        virtual bool IsServerSameZone( uint32 serverid ) = 0;
        virtual bool IsPlayerSameZone( uint32 playerid ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_zone, KFZoneInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif