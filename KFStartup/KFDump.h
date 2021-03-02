#ifndef __KF_DUMP_H__
#define __KF_DUMP_H__

#include <windows.h>
#include "dbghelp.h"

namespace KFrame
{
    class KFDump
    {
    public:
        KFDump( const char* app_name, const char* app_type, const char* app_id );
        ~KFDump();

        // 异常回调
        static LONG WINAPI MyExceptionFilter( struct _EXCEPTION_POINTERS* pExceptionInfo );
    };

}
#endif