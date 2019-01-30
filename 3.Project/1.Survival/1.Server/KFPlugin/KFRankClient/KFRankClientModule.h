#ifndef __KF_RANK_CLIENT_MODULE_H__
#define __KF_RANK_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    帮派数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-12
************************************************************************/

#include "KFRankClientInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRankClientConfig.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFRankClientModule : public KFRankClientInterface
    {
    public:
        KFRankClientModule() = default;
        ~KFRankClientModule() = default;

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();
        virtual void AfterRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnDataUpdateCallBack );

    protected:
        // 请求查询全服排行榜
        __KF_MESSAGE_FUNCTION__( HandleQueryRankListReq );

        // 请求排行榜数据返回
        __KF_MESSAGE_FUNCTION__( HandleQueryRankListAck );

        // 请求好友排行榜
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRankListReq );

        // 请求排行榜数据返回
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRankListAck );

    protected:


        // 发送消息到Rank
        bool SendMessageToRank( uint32 rankid, uint32 msgid, ::google::protobuf::Message* message );

        // 更新排行榜数据
        void UpdateRankDataToShard( KFEntity* player, const KFRankSetting* kfsetting );

        // 计算排行榜的打榜分值
        uint64 CalcRankDataScore( KFEntity* player, const KFRankSetting* kfsetting );

        // 计算zoneid
        uint32 CalcRankZoneId( uint32 playerid, const KFRankSetting* kfsetting );

    private:
        // 玩家组件
        KFComponent* _kf_component{ nullptr };

        // 更新的排行榜操作
        std::map< uint32, std::set< KFRankSetting* > > _update_rank_list;
    };
}

#endif