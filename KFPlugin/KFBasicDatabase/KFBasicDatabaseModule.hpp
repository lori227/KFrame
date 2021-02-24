#ifndef __KF_BASIC_DATABASE_MODULE_H__
#define __KF_BASIC_DATABASE_MODULE_H__

/************************************************************************
//    @Module			:    公共数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFBasicDatabaseInterface.h"
#include "KFBasicDatabaseRedis.hpp"
#include "KFBasicDatabaseMongo.hpp"

namespace KFrame
{
    class KFBasicDatabaseModule : public KFBasicDatabaseInterface
    {
    public:
        KFBasicDatabaseModule() = default;
        ~KFBasicDatabaseModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        virtual void UpdateBasicIntValue( uint64 player_id, uint64 server_id, const StringUInt64& values );
        virtual void UpdateBasicStrValue( uint64 player_id, uint64 server_id, const StringMap& values );

        // 查询玩家id
        virtual uint64 QueryBasicPlayerid( const std::string& playername, uint32 zone_id );

        // 清除玩家的在线服务器id
        virtual void ClearBasicServerId( uint64 server_id );

        // 查询玩家的服务器id
        virtual uint64 QueryBasicServerId( uint64 player_id );

        // 查询玩家基础信息
        virtual uint32 QueryBasicAttribute( uint64 player_id, StringMap& values );
        virtual uint32 QueryBasicAttribute( const std::string& playername, uint32 zone_id, StringMap& values );

        // 设置玩家名字
        virtual uint32 SetPlayerName( uint32 zone_id, uint64 player_id, const std::string& oldname, const std::string& newname );

        // 查询属性
        virtual uint64 QueryBasicIntValue( uint64 player_id, const std::string& data_name );
        virtual std::string QueryBasicStrValue( uint64 player_id, const std::string& data_name );
    private:
        // 属性逻辑
        KFBasicDatabaseLogic* _basic_database_logic = nullptr;
    };
}



#endif