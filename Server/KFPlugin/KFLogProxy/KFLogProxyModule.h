/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_PROXY_MODULE__
#define __KF_LOG_PROXY_MODULE__

#include "KFrame.h"
#include "KFConHash/KFConHash.h"
#include "KFProtocol/KFProtocol.h"
#include "KFLogProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{

    class KFLogProxyModule : public KFLogProxyInterface
    {
    public:
        KFLogProxyModule();
        ~KFLogProxyModule();

        // 鍔犺浇閰嶇疆
        virtual void InitModule();

        // 鍒濆鍖?
        virtual void BeforeRun();

        // 鍏抽棴
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    };

}

#endif //!__KF_LOG_PROXY_MODULE__