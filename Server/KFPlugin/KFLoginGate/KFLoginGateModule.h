#ifndef __KF_LOGIN_GATE_MODULE_H__
#define __KF_LOGIN_GATE_MODULE_H__

/************************************************************************
//    @Moudle			:    登录网关模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFLoginGateInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFGate/KFGateInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFLoginGateModule : public KFLoginGateInterface
    {
    public:
        KFLoginGateModule();
        ~KFLoginGateModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 登录验证
        __KF_MESSAGE_FUNCTION__( HandleLoginVerifyReq );

        // 验证结果
        __KF_MESSAGE_FUNCTION__( HandleLoginVerifyAck );

    protected:
        // 更新ip地址到login
        __KF_TIMER_FUNCTION__( OnTimerUpdateClientToLogin );

    protected:
        // 更新login回调
        void OnHttpLoginUpdateCallBack( std::string& senddata, std::string& recvdata );

    };
}



#endif