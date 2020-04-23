#ifndef __KF_BASIC_ATTRIBUTE_LOGIC_H__
#define __KF_BASIC_ATTRIBUTE_LOGIC_H__

#include "KFrame.h"

namespace KFrame
{
    class KFBasicAttributeLogic
    {
    public:
        KFBasicAttributeLogic() = default;
        virtual ~KFBasicAttributeLogic() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新属性
        virtual void UpdateBasicIntValue( uint64 playerid, uint64 serverid, const StringUInt64& values ) = 0;
        virtual void UpdateBasicStrValue( uint64 playerid, uint64 serverid, const StringMap& values ) = 0;

        // 查询玩家id
        virtual uint64 QueryBasicPlayerid( const std::string& playername, uint32 zoneid ) = 0;

        // 清除玩家的在线服务器id
        virtual void ClearBasicServerId( uint64 serverid ) = 0;

        // 查询玩家的服务器id
        virtual uint64 QueryBasicServerId( uint64 playerid ) = 0;

        // 查询玩家基础信息
        virtual uint32 QueryBasicAttribute( uint64 playerid, StringMap& values ) = 0;

        // 设置玩家名字
        virtual uint32 SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname ) = 0;

        // 查询属性
        virtual uint64 QueryBasicIntValue( uint64 playerid, const std::string& dataname ) = 0;
        virtual std::string QueryBasicStrValue( uint64 playerid, const std::string& dataname ) = 0;

    };
}



#endif