#ifndef __KF_DIR_DATABASE_MODULE_H__
#define __KF_DIR_DATABASE_MODULE_H__

/************************************************************************
//    @Module			:    目录数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFDirDatabaseInterface.h"
#include "KFDirDatabaseRedis.hpp"
#include "KFDirDatabaseMongo.hpp"

namespace KFrame
{
    class KFDirDatabaseModule : public KFDirDatabaseInterface
    {
    public:
        KFDirDatabaseModule() = default;
        ~KFDirDatabaseModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 小区注册
        virtual bool ZoneRegister( KFJson& zone_data );

        // 小区更新
        virtual bool ZoneUpdate( KFJson& zone_data );

        // 查询小区列表
        virtual StringMapList QueryZoneList( const std::string& flag );

        // 查询小区ip
        virtual StringMap QueryZoneIp( uint32 zone_id );

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zone_id, uint32 count );

        // 设置推荐小区
        virtual bool SetZoneRecommend( const std::string& flag, uint32 zone_id, bool is_recommend );

        // 分配玩家小区
        virtual StringMap AllocPlayerZone( const std::string& flag, uint32 zone_id );

        // 查询小区状态
        virtual uint32 QueryZoneStatus( uint32 zone_id );

        // 更新小区状态
        virtual bool UpdateZoneStatus( uint32 zone_id, uint32 status );

        // world http地址注册
        virtual bool SetWorldUrl( uint64 world_id, const std::string& url );

        // 获得world url
        virtual std::string GetWorldUrl( uint64 world_id );

        // 更新masterip
        virtual bool UpdateMasterIp( KFJson& master_data );

        // 查询masterip
        virtual StringMap QueryMasterIp( KFJson& master_data );

        // 查询masterlist
        virtual StringMapList QueryMasterList( KFJson& master_data );
    protected:
        KFDirDatabaseLogic* _dir_database_logic = nullptr;
    };
}



#endif