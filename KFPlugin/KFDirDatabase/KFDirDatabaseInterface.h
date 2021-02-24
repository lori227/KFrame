#ifndef __KF_DIR_DATABASE_INTERFACE_H__
#define __KF_DIR_DATABASE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDirDatabaseInterface : public KFModule
    {
    public:
        // 小区注册
        virtual bool ZoneRegister( KFJson& zone_data ) = 0;

        // 小区更新
        virtual bool ZoneUpdate( KFJson& zone_data ) = 0;

        // 查询小区列表
        virtual StringMapList QueryZoneList( const std::string& flag ) = 0;

        // 查询小区ip
        virtual StringMap QueryZoneIp( uint32 zone_id ) = 0;

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zone_id, uint32 count ) = 0;

        // 设置推荐小区
        virtual bool SetZoneRecommend( const std::string& flag, uint32 zone_id, bool is_recommend ) = 0;

        // 分配玩家小区
        virtual StringMap AllocPlayerZone( const std::string& flag, uint32 zone_id ) = 0;

        // 查询小区状态
        virtual uint32 QueryZoneStatus( uint32 zone_id ) = 0;

        // 更新小区状态
        virtual bool UpdateZoneStatus( uint32 zone_id,  uint32 status ) = 0;

        // world http地址注册
        virtual bool SetWorldUrl( uint64 world_id, const std::string& url ) = 0;

        // 获得world url
        virtual std::string GetWorldUrl( uint64 world_id ) = 0;

        // 更新masterip
        virtual bool UpdateMasterIp( KFJson& master_data ) = 0;

        // 查询masterip
        virtual StringMap QueryMasterIp( KFJson& master_data ) = 0;

        // 查询masterlist
        virtual StringMapList QueryMasterList( KFJson& master_data ) = 0;
    };
    /////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_dir_database, KFDirDatabaseInterface );
    /////////////////////////////////////////////////////////////////////////////////
}


#endif