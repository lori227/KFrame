#ifndef __KF_ENTER_CONFIG_H__
#define __KF_ENTER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////

    class KFEnterConfig : public KFConfig, public KFSingleton< KFEnterConfig >
    {
    public:
        KFEnterConfig();
        ~KFEnterConfig();

        bool LoadConfig( const char* file );

    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto* _kf_enter_config = KFEnterConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif