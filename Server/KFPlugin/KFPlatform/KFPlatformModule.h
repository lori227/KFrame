#ifndef __KF_PLATFORM_MODULE_H__
#define __KF_PLATFORM_MODULE_H__

/************************************************************************
//    @Module			:    平台模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-2-7
************************************************************************/

#include "KFrame.h"
#include "KFPlatformInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFPlatformModule : public KFPlatformInterface
    {
    public:
        KFPlatformModule();
        ~KFPlatformModule();

        // 加载配置
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 登录token
        __KF_HTTP_FUNCTION__( HandleLoginToken );

        // 激活账号
        __KF_HTTP_FUNCTION__( HandleActivationAccount );

        // 验证token
        __KF_HTTP_FUNCTION__( HandleVerifyToken );

        // 大厅信息
        __KF_HTTP_FUNCTION__( HandleUpdateZone );

        // zone http server
        __KF_HTTP_FUNCTION__( HandleZoneHttp );

        // 大厅丢失
        __KF_HTTP_FUNCTION__( HandleLostZone );

        // 更新大厅状态
        __KF_HTTP_FUNCTION__( HandleUpdateZoneStatus );

        // 查询服务器列表
        __KF_HTTP_FUNCTION__( HandleQueryZoneList );

        // 查询状态
        __KF_HTTP_FUNCTION__( HandleQueryZoneStatus );

        // 更新在线信息
        __KF_HTTP_FUNCTION__( HandleUpdateOnline );

    protected:
        void Run();

        // 查询创建账号
        MapString QueryCreateAccount( const std::string& account, uint32 channel );

        // 保存账号token
        std::string SaveLoginToken( uint32 accountid, MapString& accountdata );

        // 保存额外数据
        void UpdateExtendData( uint32 accountid, uint32 channel, KFJson& kfjson );

    protected:
        // 服务器列表类型
        uint32 _server_list_type;
    };
}



#endif