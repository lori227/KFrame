#ifndef __KF_DEPLOY_COMMAND_MODULE_H__
#define __KF_DEPLOY_COMMAND_MODULE_H__

/************************************************************************
//    @Module			:    命令模块, 处理一些GM运营命令
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-7-9
************************************************************************/

#include "KFrame.h"
#include "KFDeployCommandInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFDeployCommandModule : public KFDeployCommandInterface
    {
    public:
        KFDeployCommandModule();
        ~KFDeployCommandModule();

        // 初始化
        virtual void InitModule();

        // 开始执行
        virtual void BeforeRun();

        // 准备关闭
        virtual void BeforeShut();

        ///////////////////////////////////////////////////////////////////////////////////////
        // 关闭服务
        virtual void ShutDownServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 delaytime );

    protected:
        virtual void AddShutDownFunction( const std::string& module, KFCommandFunction& function );
        virtual void RemoveShutDownFunction( const std::string& module );

        // 判断是不是自己
        bool IsSelfServer( const std::string& appname, const std::string& apptype, uint32 appid );
    protected:
        // 处理关闭服务器
        __KF_MESSAGE_FUNCTION__( HandleShutDownServerToServerReq );

    protected:
        // 准备关闭服务器
        __KF_TIMER_FUNCTION__( OnTimerShutDownPrepare );

        // 关闭服务器
        __KF_TIMER_FUNCTION__( OnTimerShutDownServer );
    private:
        // 关闭服务器回调函数
        KFBind< std::string, const std::string&, KFCommandFunction > _shutdown_function;

    };
}



#endif