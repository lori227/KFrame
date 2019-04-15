#include "KFServices.h"
#include "KFApplication.h"

#if __KF_SYSTEM__ == __KF_LINUX__
    #include <unistd.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <signal.h>
    #include<sys/prctl.h>
#endif

namespace KFrame
{
    KFApplication::KFApplication()
    {
        _kf_services = nullptr;

#if __KF_SYSTEM__ == __KF_WIN__
        _in_handle = INVALID_HANDLE_VALUE;
#endif
    }

    KFApplication::~KFApplication()
    {
        delete _kf_services;
    }

    void KFApplication::initialize( Poco::Util::Application& self )
    {
        ServerApplication::loadConfiguration();
        ServerApplication::initialize( self );
    }

    void KFApplication::uninitialize()
    {
        ServerApplication::uninitialize();
    }

    int KFApplication::main( const std::vector< std::string >& args )
    {
        MapString params;
        for ( auto arg : args )
        {
            auto key = KFUtility::SplitString( arg, "=" );
            params[ key ] = arg;
        }

        auto daemon = params[ __KF_STRING__( daemon ) ];
        if ( daemon == "" || daemon == "1" )
        {
            SetAppDaemon();
        }

        SetupAppConsole();

        _kf_services = new KFServices();
        if ( !_kf_services->InitService( this, params ) )
        {
            SetAppPause();
            return Poco::Util::Application::EXIT_CONFIG;
        }

        // 键盘消息
        ProcessKeyEvent();

        // 等待服务关闭
        while ( !_kf_services->IsShutDown() )
        {
            KFThread::Sleep( 20 );
        }

        // 进程结束
        ServerApplication::terminate();
        return Poco::Util::Application::EXIT_OK;
    }

    void KFApplication::ProcessKeyEvent()
    {
        while ( KFGlobal::Instance()->_app_run )
        {
            // 键盘事件
            DoConsoleInputEvent();

            //RunUpdate();

            KFThread::Sleep( 20 );
        }
    }

    void KFApplication::SetAppPause()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        system( "pause" );
#else
        //pause();
#endif

    }

    void KFApplication::SetAppDaemon()
    {
#if __KF_SYSTEM__ == __KF_LINUX__
        daemon( 1, 0 );

        // ignore signals
        signal( SIGINT, SIG_IGN );
        signal( SIGHUP, SIG_IGN );
        signal( SIGQUIT, SIG_IGN );
        signal( SIGPIPE, SIG_IGN );
        signal( SIGTTOU, SIG_IGN );
        signal( SIGTTIN, SIG_IGN );
        signal( SIGTERM, SIG_IGN );
#endif
    }

    void KFApplication::SetupAppConsole()
    {
#if __KF_SYSTEM__ == __KF_WIN__

        _in_handle = GetStdHandle( STD_INPUT_HANDLE );

        // 关闭×按钮 防止误点击
        HWND hWnd = GetConsoleWindow();
        if ( hWnd )
        {
            HMENU hMenu = GetSystemMenu( hWnd, FALSE );
            EnableMenuItem( hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND );
        }
#endif
    }

    void KFApplication::SetTitleText( const char* text )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        SetConsoleTitle( text );
#else
        prctl( PR_SET_NAME, text );
#endif
    }

    void KFApplication::SetAppStop()
    {
        KFGlobal::Instance()->_app_run = false;
    }

    void KFApplication::DoConsoleInputEvent()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        static const uint32 _console_buff_size = 32;
        INPUT_RECORD record[ _console_buff_size ];
        DWORD eventcount = 0;

        GetNumberOfConsoleInputEvents( _in_handle, &eventcount );
        if ( eventcount == 0 )
        {
            return;
        }

        ReadConsoleInput( _in_handle, record, _console_buff_size, &eventcount );

        std::list< uint32 > listkeycode;
        for ( DWORD i = 0; i < eventcount; ++i )
        {
            auto event = &record[ i ];
            if ( event->EventType == KEY_EVENT && event->Event.KeyEvent.bKeyDown )
            {
                listkeycode.push_back( event->Event.KeyEvent.wVirtualKeyCode );
            }
        }

        if ( listkeycode.empty() )
        {
            return;
        }

        for ( auto keycode : listkeycode )
        {
            ProcessInputKeyEvent( keycode );
        }

#endif
    }

    void KFApplication::ProcessInputKeyEvent( uint32 keycode )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        switch ( keycode )
        {
        case VK_F1:
        {
            std::cout << "=============输入exit关闭程序==================\n";
            std::cout << ">>";

            std::string command;
            std::cin >> command;
            if ( command == "exit" )
            {
                KFGlobal::Instance()->_app_run = false;
                KFThread::Sleep( 2000 );
            }
            else
            {
                KFPluginManage::Instance()->AddCommand( command );
            }
            break;
        }
        default:
            break;
        }
#endif

    }

}
