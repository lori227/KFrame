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
#include "KFConfig/KFZoneConfig.hpp"

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
        void OnRouteConnectCluster( uint64 server_id );

        // 更新属性回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateStringCallBack );

        // 上线更新公共数据
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterUpdateBasicAttribute );

        // 离线更新公共数据
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveUpdateBasicAttribute );

        // 上下线更新玩家数据
        void EnterLeaveUpdateAttributeToBasic( EntityPtr player );


        // 更新公共数据
        void UpdateBasicIntValueToBasic( uint64 player_id, StringUInt64& values );
        void UpdateBasicIntValueToBasic( uint64 player_id, const std::string& data_name, uint64 data_value );
        void UpdateBasicStrValueToBasic( uint64 player_id, const std::string& data_name, const std::string& data_value );

        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name, uint32 maxlength );
    protected:
        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq, KFMsg::MsgSetSexReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq, KFMsg::MsgSetNameReq );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToGameAck, KFMsg::S2SSetPlayerNameToGameAck );

        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicReq, KFMsg::MsgQueryBasicReq );

        // 查询玩家基本数据
        __KF_MESSAGE_FUNCTION__( HandleQueryAttributeToGameAck, KFMsg::S2SQueryAttributeToGameAck );
    private:
        // 组件
        std::shared_ptr<KFComponent> _kf_component = nullptr;

        // 是否需要重置在线数据
        bool _need_refresh_online = true;
    };
}

#endif