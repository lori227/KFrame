 #ifndef __KF_COMMAND_MODULE_H__
#define __KF_COMMAND_MODULE_H__

/************************************************************************
//    @Module			:    命令模块, 处理一些GM运营命令
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-16
************************************************************************/

#include "KFrame.h"
#include "KFCommandInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFCommandModule : public KFCommandInterface
    {
    public:
        KFCommandModule();
        ~KFCommandModule();

        // 初始化
        virtual void InitModule();

        // 开始执行
        virtual void BeforeRun();

        // 准备关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 添加命令
        virtual void AddCommand( const char* appname, uint32 appid, const char* command );

        // 卸载
        virtual void UnRegisterCommandFunction( const char* command );

    protected:
        // 注册函数
        virtual void RegisterFunction( const char* command, KFCommandFunction& function );

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////


        // 执行命令
        void ExecuteCommand( const std::string& command );

        // 加载配置
        void ExecuteLoadConfig( KFJson& json );

        // 关闭服务
        void ExecuteShutDown( KFJson& json );

    protected:
        // 查询命令
        __KF_TIMER_FUNCTION__( OnTimerQueryCommand );

        // 服务器关闭
        __KF_TIMER_FUNCTION__( OnTimerServiceShutDown );
    protected:

        // 命令函数
        KFBind< std::string, const std::string&, KFCommandFunction > _kf_command_function;
    };
}



#endif