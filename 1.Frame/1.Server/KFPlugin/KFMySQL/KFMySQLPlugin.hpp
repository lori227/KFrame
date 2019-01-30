#ifndef __KF_MYSQL_PLUGIN_H__
#define __KF_MYSQL_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFMySQLPlugin : public KFPlugin
    {
    public:
        virtual void Install();

        virtual void UnInstall();

        virtual void LoadModule();
    };
}


#endif