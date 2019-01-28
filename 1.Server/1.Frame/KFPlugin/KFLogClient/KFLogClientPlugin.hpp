/*------------------------------------
// Module:
// Author: NickYang
// Mail: NickYang1988@qq.com
// Date: 2018/06/28
------------------------------------*/
#ifndef __KF_LOG_CLIENT_PLUGIN__
#define __KF_LOG_CLIENT_PLUGIN__

#include "KFrame.h"

namespace KFrame
{
    class KFLogClientPlugin : public KFPlugin
    {
    public:
        virtual void Install();
        virtual void UnInstall();
        virtual void LoadModule();

    };
}

#endif //!__KF_LOG_CLIENT_PLUGIN__