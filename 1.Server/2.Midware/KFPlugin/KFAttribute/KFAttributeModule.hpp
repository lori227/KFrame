#ifndef __KF_ATTRIBUTE_MODULE_H__
#define __KF_ATTRIBUTE_MODULE_H__

/************************************************************************
//    @Module			:    属性逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-12-27
************************************************************************/

#include "KFProtocol/KFProtocol.h"
#include "KFAttributeInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFDebug/KFDebugInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFDataClient/KFDataClientInterface.h"

namespace KFrame
{
    class KFAttributeModule : public KFAttributeInterface
    {
    public:
        KFAttributeModule() = default;
        ~KFAttributeModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加属性
        __KF_DEBUG_FUNCTION__( DebugAddData );

        // 减少属性
        __KF_DEBUG_FUNCTION__( DebugDecData );

        // 设置属性
        __KF_DEBUG_FUNCTION__( DebugSetData );


    protected:
        // 查询玩家所有数据
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq );

        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq );

        // 请求修改人物头像
        __KF_MESSAGE_FUNCTION__( HandleChangeIconReq );

        // 处理修改头像框请求
        __KF_MESSAGE_FUNCTION__( HandleChangeIconBoxReq );

        // 请求修改个性签名
        __KF_MESSAGE_FUNCTION__( HandleChangeMottoReq );

        // 删除属性请求
        __KF_MESSAGE_FUNCTION__( HandleRemoveDataReq );

        // 查询玩家设置请求
        __KF_MESSAGE_FUNCTION__( HandleQuerySettingReq );

        // 玩家更新设置请求
        __KF_MESSAGE_FUNCTION__( HandleUpdateSettingReq );

        // 更换称号
        __KF_MESSAGE_FUNCTION__( HandleTitleChangeReq );

    protected:
        // 查询玩家数据
        void OnAfterQueryPlayerData( uint32 result, uint64 playerid, KFMsg::PBObject* playerdata );

        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name );

        // 设置名字回调
        void OnAfterSetPlayerName( uint32 result, KFEntity* player, const std::string& name, uint64 itemguid );
    };
}



#endif