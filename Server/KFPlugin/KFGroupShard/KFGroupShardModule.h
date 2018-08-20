#ifndef __KF_GROUP_SHARD_MODULE_H__
#define __KF_GROUP_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    匹配小组逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-4-4
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFGroupShardInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFGroupShardModule : public KFGroupShardInterface
    {
    public:
        KFGroupShardModule();
        ~KFGroupShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 服务器发现客户端
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

        // 发现代理服务器
        void OnServerDiscoverGroupProxy( uint32 proxyid );

    protected:
        // 创建队伍
        __KF_MESSAGE_FUNCTION__( HandleCreateMatchGroupToShardReq );

        // 队员加入队伍
        __KF_MESSAGE_FUNCTION__( HandleAddMatchGroupMemberReq );

        // 申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleApplyMatchGroupReq );

        // 玩家离开队伍
        __KF_MESSAGE_FUNCTION__( HandleLeaveMatchGroupReq );

        // 踢出玩家
        __KF_MESSAGE_FUNCTION__( HandleKickMatchGroupReq );

        // 上线查询队伍
        __KF_MESSAGE_FUNCTION__( HandleOnLineQueryGroupReq );

        // 离线更新队伍
        __KF_MESSAGE_FUNCTION__( HandleOffLineUpdateGroupReq );

        // 更新基础属性
        __KF_MESSAGE_FUNCTION__( HandleUpdateGroupMemberReq );

        // 更新组队模式
        __KF_MESSAGE_FUNCTION__( HandleUpdateGroupMatchReq );

    protected:
        // 发送消息给队员
        bool SendMessageToMember( KFData* kfmember, uint32 msgid, ::google::protobuf::Message* message );
        void SendMessageToGroup( KFData* kfmemberrecord, uint32 msgid, ::google::protobuf::Message* message, uint32 excludeid = 0 );

        // 队伍选择新队长
        KFData* ChooseNewGroupCaptain( KFData* kfmemberrecord );

        // 处理选择队长
        bool ProcessChooseGroupCaptain( KFEntity* kfgroup );

        // 删除队伍
        void ProcessRemoveGroup( uint64 groupid );

        // 更新基础属性
        void SendUpdateMemberToGroup( uint64 groupid, KFData* kfmemberrecord, KFData* kfmember, const std::string& dataname, const KFMsg::PBStrings& pbstrings );

        // 发送队伍信息给队员
        void SendGroupDataToMember( KFData* kfmemberrecord, KFData* kfnewmember, KFEntity* kfgroup, bool newadd );
    private:
        // 队伍组件
        KFComponent* _kf_component;
    };
}



#endif