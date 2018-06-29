#ifndef __KF_LAUNCH_MODULE_H__
#define __KF_LAUNCH_MODULE_H__

/************************************************************************
//    @Module			:    Launch模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-15
************************************************************************/

#include "KFrame.h"
#include "KFInterfaces.h"
#include "KFLaunchInterface.h"

namespace KFrame
{
    class KFLaunchSetting;
    class KFLaunchModule : public KFLaunchInterface
    {
    public:
        KFLaunchModule();
        ~KFLaunchModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        void Run();

        // 守护定时器
        void RunLaunch();

        // 绑定
        void BindLaunch();

        // 更新Launch数据到服务器
        void UpdateLaunchToDatabase();

        // 更新服务器状态
        void UpdateServiceStatus( KFLaunchSetting* kfsetting );
        void UpdateServiceStatus();
        ///////////////////////////////////////////////////////////////////////////////
        // 开启服务
        std::string HandleStartup( const std::string& ip, const std::string& data );

        // 关闭服务
        std::string HandleShutDown( const std::string& ip, const std::string& data );

        // ftp下载
        std::string HandleFtpDownload( const std::string& ip, const std::string& data );

        // 转发
        std::string HandleTransmitCommand( const std::string& ip, const std::string& data );
        ///////////////////////////////////////////////////////////////////////////////

        // 计划关闭
        void ScheduleShutDown( uint32 id, const char* data, uint32 size );

        // 计划开启
        void ScheduleStartup( uint32 id, const char* data, uint32 size );

        // 计划下载
        void ScheduleFtpDownload( uint32 id, const char* data, uint32 size );
        void FtpDownLoadFinish( uint32 result );

        // 计划转发
        void ScheduleTransmitCommand( uint32 id, const char* data, uint32 size );
        ///////////////////////////////////////////////////////////////////////////////

        // 关闭进程
        void KillProcess( const std::string& titletext );
        void KillProcess( uint32 processid );

#if __KF_SYSTEM__ == __KF_WIN__
        // 关闭程序
        void KillWinProcess( const std::string& titletext );
        void KillWinProcess( uint32 processid );

        // 绑定程序
        void BindWinLaunch();

        // 判断进程
        void CheckWinProcess( KFLaunchSetting* kfsetting );

        // 开启进程
        bool StartWinProcess( KFLaunchSetting* kfsetting );
#else
        // 关闭程序
        void KillLinuxProcess( const std::string& titletext );
        void KillLinuxProcess( uint32 processid );

        //绑定程序
        void BindLinuxLaunch();

        //判断进程
        void CheckLinuxProcess( KFLaunchSetting* kfsetting );

        //开启进程
        bool StartLinuxProcess( KFLaunchSetting* kfsetting );

        //由进程名获取PID
        void GetPidByName( uint32& pid, const char* titletext );
#endif

        /////////////////////////////////////////////////////
    protected:
        KFClockTimer _launch_timer;

    };
}



#endif
