#ifndef __KF_RANK_CLIENT_MODULE_H__
#define __KF_RANK_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    帮派数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFRankClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZone/KFZoneInterface.h"

namespace KFrame
{
    class KFRankClientModule : public KFRankClientInterface
    {
    public:
        KFRankClientModule();
        ~KFRankClientModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Rank
        virtual bool SendMessageToRank( uint64 zoneid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到Rank
        virtual bool SendMessageToRankShard( uint32 msgid, ::google::protobuf::Message* message );
        // 更新排行榜
        virtual void JoinRankList( uint64 matchid, uint32 playerid, KFEntity* player, const std::string& scoretype );

    protected:
        // 客户端请求查询全服排行榜
        __KF_MESSAGE_FUNCTION__( HandleQueryWholeRankListReq );

        // 请求好友排行榜数据返回
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRankListReq );

        // 请求排行榜数据返回
        __KF_MESSAGE_FUNCTION__( HandleQueryRankListAck );

    private:
        // 计算排行榜的打榜分值
        void CalcRankData( KFData* kfdata, uint64& evalscore, uint64& winscore, uint64& killscore );
        void FormateRankData( KFMsg::PBScoreData* pbscores, KFData* kfdata );

        // 解析排行榜
        void SerialzeToClient( const KFMsg::PBRankData* pbrankdatas, const std::string& ranktype );
        void ParseScoreToPb( const KFMsg::PBStrings* pbstrings, const std::string& ranktype );

        // 计算评分榜数据
        void SerialzeEvalRank( uint64 score, const KFMsg::PBStrings* pbstrings );

        // 计算胜利榜数据
        void SerialzeWinRank( uint64 score, const KFMsg::PBStrings* pbstrings );

        // 计算击伤榜数据
        void SerialzeKillRank( uint64 score, const KFMsg::PBStrings* pbstrings );
    };
}

#endif