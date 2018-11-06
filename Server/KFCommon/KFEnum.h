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
            DeployUpload = 1,			// 部署upload
            DeployServer = 2,			// 部署server
            DeployAgent = 3,			// 部署Agent
            Auth = 4,					// 认证服务器
            Platform = 5,				// 账号服务器

            DataMaster = 11,			// 数据服务器master
            DataProxy = 12,				// 数据服务器proxy
            DataShard = 13,				// 数据服务器shard

            PublicMaster = 21,			// 公共属性服务器master
            PublicProxy = 22,			// 公共属性服务器proxy
            PublicShard = 23,			// 公共属性服务器shard

            RouteMaster = 31,			// 转发服务器master
            RouteProxy = 32,			// 转发服务器proxy
            RouteShard = 33,			// 转发服务器shard

            MatchMaster = 41,			// 匹配服务器master
            MatchProxy = 42,			// 匹配服务器proxy
            MatchShard = 43,			// 匹配服务器shard

            BattleMaster = 51,			// 战斗服务器master
            BattleProxy = 52,			// 战斗服务器proxy
            BattleShard = 53,			// 战斗服务器shard

            MailMaster = 61,			// 邮件服务器master
            MaileProxy = 62,			// 邮件服务器proxy
            MailShard = 63,				// 邮件服务器shard

            RelationMaster = 71,		// 关系服务器master
            RelationProxy = 72,			// 关系服务器proxy
            RelationShard = 73,			// 关系服务器shard

            GroupMaster = 81,			// 组队服务器master
            GroupProxy = 82,			// 组队服务器proxy
            GroupShard = 83,			// 组队服务器shard

            RankMaster = 91,			// 排行榜服务器master
            RankProxy = 92,				// 排行榜服务器proxy
            RankShard = 93,				// 排行榜服务器shard

            GuildMaster = 101,			// 公会服务器master
            GuildProxy = 102,			// 公会服务器proxy
            GuildShard = 103,			// 公会服务器shard

            LogMaster = 111,			// 日志服务器master
            LogProxy = 112,				// 日志服务器proxy
            LogShard = 113,				// 日志服务器shard

            DirMaster = 121,			// 目录服务器master
            DirProxy = 122,				// 目录服务器proxy
            DirShard = 123,				// 目录服务器shard

            BattleServer = 131,         // 战斗服务器( gs )

            ZoneMaster = 241,			// 小区服务器master
            ZoneWorld = 242,			// 小区服务器world
            ZoneGate = 243,				// 小区服务器gate
            ZoneLogin = 244,			// 小区服务器login
            ZoneGame = 245,				// 小区服务器game
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