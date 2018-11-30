#ifndef __KF_ENUM_H__
#define __KF_ENUM_H__

namespace KFrame
{
    namespace KFEnum
    {
        enum MyEnum
        {
            Error = 0,	// 错误
            Ok = 1,		// 成功

            //////////////////////////////////////
            // 任务 成就系统
            UseOperate = 1,
            UseFinal = 2,

            Lobby = 1,		// 大厅类型
            Battle = 2,		// 战服类型
            //////////////////////////////////////
        };
    }
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    // 消息类型
    namespace KFMessageEnum
    {
        enum MyEnum
        {
            Server = 1,		// 服务器与服务器之间消息
            Client = 2,		// 客户端与服务器之间消息
        };
    }

    namespace KFServerEnum
    {
        enum MyEnum
        {
            TotalServerList = 1,	// 所有服务器列表
            SelectServerData = 2,	// 选择服务器登录
            ///////////////////////////////////////////

            Local = 1,			// 内网
            Internet = 2,		// 外网
            ///////////////////////////////////////////

            Debug = 1,			// 调试
            Develop = 2,		// 研发
            Beta = 3,			// 测试
            Audit = 4,			// 审核
            Online = 8,			// 正式
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            DeployServer = 1,			// 部署server
            DeployAgent = 2,			// 部署Agent
            BattleServer = 3,			// 战斗服务器( gs )

            Auth = 10,					// 认证服务器

            DataMaster = 11,			// 数据服务器master
            DataProxy = 12,				// 数据服务器proxy
            DataShard = 13,				// 数据服务器shard

            PublicMaster = 14,			// 公共属性服务器master
            PublicProxy = 15,			// 公共属性服务器proxy
            PublicShard = 16,			// 公共属性服务器shard

            RouteMaster = 17,			// 转发服务器master
            RouteProxy = 18,			// 转发服务器proxy
            RouteShard = 19,			// 转发服务器shard

            MatchMaster = 21,			// 匹配服务器master
            MatchProxy = 22,			// 匹配服务器proxy
            MatchShard = 23,			// 匹配服务器shard

            BattleMaster = 24,			// 战斗服务器master
            BattleProxy = 25,			// 战斗服务器proxy
            BattleShard = 26,			// 战斗服务器shard

            MailMaster = 27,			// 邮件服务器master
            MaileProxy = 28,			// 邮件服务器proxy
            MailShard = 29,				// 邮件服务器shard

            RelationMaster = 31,		// 关系服务器master
            RelationProxy = 32,			// 关系服务器proxy
            RelationShard = 33,			// 关系服务器shard

            GroupMaster = 34,			// 组队服务器master
            GroupProxy = 35,			// 组队服务器proxy
            GroupShard = 36,			// 组队服务器shard

            RankMaster = 37,			// 排行榜服务器master
            RankProxy = 38,				// 排行榜服务器proxy
            RankShard = 39,				// 排行榜服务器shard

            GuildMaster = 41,			// 公会服务器master
            GuildProxy = 42,			// 公会服务器proxy
            GuildShard = 43,			// 公会服务器shard

            LogMaster = 44,				// 日志服务器master
            LogProxy = 45,				// 日志服务器proxy
            LogShard = 46,				// 日志服务器shard

            DirMaster = 47,				// 目录服务器master
            DirProxy = 48,				// 目录服务器proxy
            DirShard = 49,				// 目录服务器shard

            ZoneMaster = 91,			// 小区服务器master
            ZoneWorld = 92,				// 小区服务器world
            ZoneGate = 93,				// 小区服务器gate
            ZoneLogin = 94,				// 小区服务器login
            ZoneGame = 95,				// 小区服务器game
        };
    }


    namespace KFThreadEnum
    {
        enum MyEnum
        {
            ST = 1,		// 单线程
            MT = 2,		// 多线程
        };
    }

    namespace KFBufferEnum
    {
        enum MyEnum
        {
            Buff_1M = 1024 * 1024,		// buff大小
            Buff_10M = 10 * Buff_1M,
            Buff_20M = 20 * Buff_1M,
            Buff_40M = 40 * Buff_1M,
            Buff_100M = 100 * Buff_1M,
            Buff_200M = 200 * Buff_1M,
        };
    }
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    namespace KFOperateEnum
    {
        enum EConstDefine
        {
            Add = 1,		// +
            Dec = 2,		// -
            Set = 3,		// =
            And = 4,		// &
            ABit = 5,		// |
            XBit = 6,		// &~
            Mul = 7,		// *
            Div = 8,		// /


            Greater = 11,	// >=
            Less = 12,		// =<
            Equal = 13,		// ==
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFZoneEnum
    {
        enum MyEnum
        {
            MaxPlayerCount = 1000000,	// 每个区最大的角色数量
        };
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFRankEnum
    {
        enum MyEnum
        {
            TotalRank = 1,		// 全区全服排行榜
            ZoneRank = 2,		// 分区排行榜
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFRatioEnum
    {
        enum MyEnum
        {
            Ratio = 10000,	// 积分比率
        };
    }

    namespace KFCompressEnum
    {
        enum MyEnum
        {
            None = 0x0,			// 无操作
            Convert = 0x1,		// 转换
            Zib = 0x2,			// 压缩
            Compress = 0x3,		// 压缩转换
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif