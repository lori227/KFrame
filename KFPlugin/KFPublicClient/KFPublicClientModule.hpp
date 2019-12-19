#ifndef __KF_PUBLIC_CLIENT_MODULE_H__
#define __KF_PUBLIC_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    公共数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFPublicClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFPublicClientModule : public KFPublicClientInterface
    {
    public:
        KFPublicClientModule() = default;
        ~KFPublicClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 连接成功
        void OnRouteConnectCluster( uint64 serverid );

        // 更新属性回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateStringCallBack );

        // 上线更新公共数据
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterUpdatePublicData );

        // 离线更新公共数据
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveUpdatePublicData );

        // 更新数据
        void OnUpdateDataToPublic( KFEntity* player, KFData* kfdata );

        // 更新公共数据
        void UpdatePublicData( uint64 playerid, StringMap& values );
        void UpdatePublicData( KFEntity* player, StringMap& values );
    protected:
        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicReq );

        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicToGameAck );
    private:
        // 组件
        KFComponent* _kf_component = nullptr;

        // 基础模块
        KFData* _kf_basic = nullptr;

        // 是否需要重置在线数据
        bool _need_refresh_online = true;
    };
}

#endif