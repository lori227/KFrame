#ifndef __KF_DIR_ATTRIBUTE_INTERFACE_H__
#define __KF_DIR_ATTRIBUTE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDirAttributeInterface : public KFModule
    {
    public:
        // 小区注册
        virtual bool ZoneRegister( KFJson& zonedata ) = 0;

        // 小区更新
        virtual bool ZoneUpdate( KFJson& zonedata ) = 0;

        // 查询小区列表
        virtual StringListMap QueryZoneList( const std::string& flag ) = 0;

        // 查询小区ip
        virtual StringMap QueryZoneIp( uint32 zoneid ) = 0;

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zoneid, uint32 count ) = 0;

        // 设置推荐小区
        virtual bool SetZoneRecommend( const std::string& flag, uint32 zoneid, bool isrecommend ) = 0;

        // 分配玩家小区
        virtual StringMap AllocPlayerZone( const std::string& flag, uint32 zoneid ) = 0;

        // world http地址注册
        virtual bool SetWorldUrl( uint64 worldid, const std::string& url ) = 0;

        // 获得world url
        virtual std::string GetWorldUrl( uint64 worldid ) = 0;

        // 更新masterip
        virtual bool UpdateMasterIp( KFJson& masterdata ) = 0;

        // 查询masterip
        virtual StringMap QueryMasterIp( KFJson& masterdata ) = 0;

        // 查询masterlist
        virtual StringListMap QueryMasterList( KFJson& masterdata ) = 0;
    };
    /////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_dir_attribute, KFDirAttributeInterface );
    /////////////////////////////////////////////////////////////////////////////////
}


#endif