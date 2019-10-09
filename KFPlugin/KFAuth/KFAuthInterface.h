#ifndef __KF_AUTH_INTERFACE_H__
#define __KF_AUTH_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFAuthInterface : public KFModule
    {
    public:
    };
    __KF_INTERFACE__( _kf_auth, KFAuthInterface );

}


#endif