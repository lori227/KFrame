#ifndef __KF_PAY_CLIENT_MODULE_H__
#define __KF_PAY_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:   充值模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFPayClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFPayConfig.hpp"

namespace KFrame
{
    class KFPayClientModule : public KFPayClientInterface
    {
    public:
        KFPayClientModule() = default;
        ~KFPayClientModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 请求生成订单
        __KF_MESSAGE_FUNCTION__( HandleApplyPayOrderReq );

        // 充值结果
        __KF_MESSAGE_FUNCTION__( HandlePayResultReq );

        // 请求查询充值信息
        __KF_MESSAGE_FUNCTION__( HandleQueryPayReq );

    protected:
        // 上线查询充值订单
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterPayModule );
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeavePayModule );

        // 开启查询充值定时器
        void StartQueryPayTimer( KFEntity* player );
        __KF_TIMER_FUNCTION__( OnTimerQueryPayData );

        // 查询充值信息
        void QueryPayData( uint64 playerid );

        // 生成订单回调
        __KF_HTTP_CALL_BACK_FUNCTION__( OnHttpApplyOrderCallBack );

        // 查询充值信息回调
        __KF_HTTP_CALL_BACK_FUNCTION__( OnHttpQueryPayCallBack );

        // 初始充值逻辑
        void ProcessPay( KFEntity* player, const std::string& payid, const std::string& order );
    };

}

#endif//__KF_PAY_MODULE_H__