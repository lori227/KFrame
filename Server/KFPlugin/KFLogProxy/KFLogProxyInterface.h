/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_PROXY_INTERFACE__
#define __KF_LOG_PROXY_INTERFACE__

#include "KFrame.h"

namespace KFrame
{

    class KFLogProxyInterface : public KFModule
    {
    public:

    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mail_proxy, KFLogProxyInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif //!__KF_LOG_PROXY_INTERFACE__