/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/07/10
------------------------------------*/
#ifndef __KF_LOG_CLIENT_MODULE__
#define __KF_LOG_CLIENT_MODULE__

#include "KFLogClientInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFZone/KFZoneInterface.h"

namespace KFrame
{
    class KFLogClientModule : public KFLogClientInterface
    {
    public :
        KFLogClientModule();
        ~KFLogClientModule();

        virtual void InitModule();
        virtual void BeforeRun();
        virtual void BeforeShut();

        virtual void LogRemote( const int log_level, const int log_category, const std::string& log_info );

    protected:
        bool SendMessageToLog( uint32 msgid, ::google::protobuf::Message* message );
    };
}


#endif //!__KF_LOG_CLIENT_MODULE__