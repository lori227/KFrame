#ifndef __KF_PLATFORM_MODULE_H__
#define __KF_PLATFORM_MODULE_H__

/************************************************************************
//    @Module			:    平台逻辑相关
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-19
************************************************************************/

#include "KFrame.h"
#include "KFPlatformInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFPlatformModule : public KFPlatformInterface
    {
    public:
        KFPlatformModule() = default;
        ~KFPlatformModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 获得api地址
        virtual const std::string& GetPlatformApiUrl();

        // 创建签名
        virtual const std::string& MakePlatformSign( uint32 nowtime );

        // 创建平台连接
        virtual const std::string& MakePlatformUrl( const std::string& path );

    protected:
        // 绑定支付宝
        __KF_MESSAGE_FUNCTION__( HandleBindAlipayReq );

        // 绑定支付宝回调
        __KF_HTTP_CALL_BACK_FUNCTION__( OnBindAlipayCallBack );

        // 查询邀请积分
        __KF_MESSAGE_FUNCTION__( HandleQueryInviteScoreReq );

        // 查询邀请积分回调
        __KF_HTTP_CALL_BACK_FUNCTION__( OnQueryInviteScoreCallBack );
        ////////////////////////////////////////////////////////////////////////////////
    };
}



#endif