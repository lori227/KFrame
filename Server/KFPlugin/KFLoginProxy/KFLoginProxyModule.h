#ifndef __KF_LOGIN_PROXY_MODULE_H__
#define __KF_LOGIN_PROXY_MODULE_H__

/************************************************************************
//    @Moudle			:    登录节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFLoginProxyInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFProxy/KFProxyInterface.h"
#include "KFGate/KFGateInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFLoginToken
    {
    public:
        KFLoginToken()
        {
            _player_id = 0;
            _game_id = 0;
            _valid_time = 0;
            _account_id = 0;
        }

    public:
        std::string _token;
        uint32 _player_id;
        uint32 _game_id;
        uint64 _valid_time;
        uint32 _account_id;
    };

    class KFLoginProxyModule : public KFLoginProxyInterface
    {
    public:
        KFLoginProxyModule();
        ~KFLoginProxyModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理更新token信息
        __KF_MESSAGE_FUNCTION__( HandleLoginTellTokenToProxyReq );

        // 验证token
        __KF_MESSAGE_FUNCTION__( HandleLoginGameReq );

        // 处理登录成功
        __KF_MESSAGE_FUNCTION__( HandleLoginGameAck );

        // 登出游戏
        __KF_MESSAGE_FUNCTION__( HandleLoginOutReq );

    protected:
        // 更新在线数量
        __KF_TIMER_FUNCTION__( OnTimerUpdateOnlineToGame );

    private:
        // 逻辑
        void Run();
        void AfterRun();

        // 检查登录token的有效性
        void RunLoginTokenValidTime();

        // 删除失效的登录token
        void RunRemoveLoginToken();

    private:
        // 认证的token列表
        KFMap< uint32, uint32, KFLoginToken > _kf_token_list;
        std::set< uint32 >_kf_remove_list;
    };
}

#endif