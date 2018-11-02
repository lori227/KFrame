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
    // 命令数目
    class KFCommand
    {
    public:
        KFBind< std::string, const std::string&, KFCommandFunction > _command_function;
    };

    class KFDeployCommandModule : public KFDeployCommandInterface
    {
    public:
        KFDeployCommandModule() = default;
        ~KFDeployCommandModule() = default;

        // 开始执行
        virtual void BeforeRun();

        // 准备关闭
        virtual void BeforeShut();

        ///////////////////////////////////////////////////////////////////////////////////////
        // 部署命令
        virtual void DeployCommand( const std::string& command, const std::string& value, const std::string& appname, const std::string& apptype, const std::string&  appid, uint32 zoneid );

    protected:
        virtual void AddCommandFunction( const std::string& command, const std::string& module, KFCommandFunction& function );
        virtual void RemoveComandFunction( const std::string& command, const std::string& module );

        // 判断是不是自己
        bool IsSelfServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid );

        // 关闭服务器
        void ShutDownServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid, uint32 delaytime );

    protected:
        // 处理关闭服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandToServerReq );

    protected:
        // 准备关闭服务器
        __KF_TIMER_FUNCTION__( OnTimerShutDownPrepare );

        // 关闭服务器
        __KF_TIMER_FUNCTION__( OnTimerShutDownServer );
    private:
        // 命令回调函数
        KFMap< std::string, const std::string&, KFCommand > _command_data;

    };
}



#endif