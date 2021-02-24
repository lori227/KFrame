#ifndef __KF_BASIC_DATABASE_MONGO_H__
#define __KF_BASIC_DATABASE_MONGO_H__

#include "KFBasicDatabaseLogic.hpp"
#include "KFMongo/KFMongoInterface.h"

namespace KFrame
{
    class KFBasicDatabaseMongo : public KFBasicDatabaseLogic
    {
    public:
        KFBasicDatabaseMongo() = default;
        ~KFBasicDatabaseMongo() = default;

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

        // 设置玩家名字
        virtual uint32 SetPlayerName( uint32 zone_id, uint64 player_id, const std::string& oldname, const std::string& newname );

        // 查询属性
        virtual uint64 QueryBasicIntValue( uint64 player_id, const std::string& data_name );
        virtual std::string QueryBasicStrValue( uint64 player_id, const std::string& data_name );
    protected:
        // 格式化名字字段
        std::string FormatNameTable( uint32 zone_id );
    };
}



#endif