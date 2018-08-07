/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_PROXY_MODULE__
#define __KF_LOG_PROXY_MODULE__

#include "KFrame.h"
#include "KFLogProxyInterface.h"

namespace KFrame
{
    class KFLogProxyModule : public KFLogProxyInterface
    {
    public:
        KFLogProxyModule();
        ~KFLogProxyModule();

        virtual void InitModule();

        virtual void BeforeRun();

        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    };

}

#endif //!__KF_LOG_PROXY_MODULE__