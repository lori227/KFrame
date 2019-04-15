#ifndef __KF_DEPLOY_DUMP_H__
#define __KF_DEPLOY_DUMP_H__

#include <windows.h>
#include "dbghelp.h"

namespace KFrame
{
    class KFDeployDump
    {
    public:
        KFDeployDump( const char* appname, const char* apptype, const char* appid );
        ~KFDeployDump();

        // 异常回调
        static LONG WINAPI MyExceptionFilter( struct _EXCEPTION_POINTERS* pexceptioninfo );
    };

}
#endif