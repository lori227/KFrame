#ifndef __KF_ENUM_H__
#define __KF_ENUM_H__

namespace KFrame
{
    namespace KFCommonEnum
    {
        enum MyEnum
        {
            OK = 1,		// 成功的结果
        };
    }
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////

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
    namespace KFScoreEnum
    {
        enum MyEnum
        {
            ScoreRatio = 10000,	// 积分比率
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
}

#endif