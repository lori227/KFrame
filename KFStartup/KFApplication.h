#ifndef __KF_APPLICATION_H__
#define __KF_APPLICATION_H__

#include "KFrame.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"

namespace KFrame
{
    class KFServices;
    class KFApplication : public Poco::Util::ServerApplication, public KFSingleton< KFApplication >
    {
    public:

        KFApplication();
        ~KFApplication();

        // 设置标题
        void SetTitleText( const char* text );

        // 设置停止服务
        void SetAppStop();

    protected:
        int main( const StringVector& args );
        void initialize( Poco::Util::Application& self );
        void uninitialize();

        // 处理键盘输入事件
        void ProcessKeyEvent();

        // 暂停
        void SetAppPause();

        // 后台运行
        void SetAppDaemon();

        // 设置
        void SetupAppConsole();

        // 读取输入
        void DoConsoleInputEvent();

        // 处理输入
        void ProcessInputKeyEvent( uint32 keycode );

        // 初始化默认参数
        void InitDefaultParams( StringMap& params );
    protected:

        // 服务
        KFServices* _kf_services;

#if __KF_SYSTEM__ == __KF_WIN__
        // 输入句柄
        HANDLE _in_handle;
#endif
    };
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static KFrame::KFApplication* _kf_appliction = nullptr;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
