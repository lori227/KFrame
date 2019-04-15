#include "KFDeployDump.h"
#include <string>

namespace KFrame
{
#pragma comment( lib, "DbgHelp" )

#pragma warning( push )
#pragma warning( disable : 4311 )

    ////////////////////////////////////////////////////////////////////////////////////////
    // 程序名称
    static char _app_name[ MAX_PATH ];
    ////////////////////////////////////////////////////////////////////////////////////////

    KFDeployDump::KFDeployDump( const char* appname, const char* apptype, const char* strappid )
    {
        memset( _app_name, 0, sizeof( _app_name ) );

        sprintf_s( _app_name, "%s-%s-%s", appname, apptype, strappid );
        ::SetUnhandledExceptionFilter( MyExceptionFilter );
    }

    KFDeployDump::~KFDeployDump()
    {

    }

    LONG KFDeployDump::MyExceptionFilter( struct _EXCEPTION_POINTERS* pExceptionInfo )
    {
        SYSTEMTIME time;
        GetLocalTime( &time );

        char szDumpPath[ _MAX_PATH ] = { 0 };
        sprintf_s( szDumpPath, sizeof( szDumpPath ), "[%s]-%02d-%02d-%02d-%02d-%02d.dmp", _app_name, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );

        HANDLE hFile = CreateFile( szDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        // Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pExceptionInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = FALSE;

        // 写入Dump文件内容
        MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &dumpInfo, NULL, NULL );

        CloseHandle( hFile );

        //FatalAppExit( -1, szDumpPath );
        return EXCEPTION_EXECUTE_HANDLER;
    }
#pragma warning( pop )
}