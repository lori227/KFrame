#ifndef __KF_BASIC_CLIENT_MODULE_H__
#define __KF_BASIC_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    玩家基础数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFBasicClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFZoneConfig.hpp"

namespace KFrame
{
    class KFBasicClientModule : public KFBasicClientInterface
    {
    public:
        KFBasicClientModule() = default;
        ~KFBasicClientModule() = default;

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
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterUpdateBasicAttribute );

        // 离线更新公共数据
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveUpdateBasicAttribute );

        // 上下线更新玩家数据
        void EnterLeaveUpdateAttributeToBasic( KFEntity* player );


        // 更新公共数据
        void UpdateBasicIntValueToBasic( uint64 playerid, KeyValue& values );
        void UpdateBasicIntValueToBasic( uint64 playerid, const std::string& dataname, uint64 datavalue );
        void UpdateBasicStrValueToBasic( uint64 playerid, const std::string& dataname, const std::string& datavalue );

        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name, uint32 maxlength );
    protected:
        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToGameAck );

        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicReq );

        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryAttributeToGameAck );
    private:
        // 组件
        KFComponent* _kf_component = nullptr;

        // 是否需要重置在线数据
        bool _need_refresh_online = true;
    };
}

#endif