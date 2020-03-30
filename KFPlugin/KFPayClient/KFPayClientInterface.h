#ifndef __KF_PAY_CLIENT_INTERFACE_H__
#define __KF_PAY_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFPayClientInterface : public KFModule
    {
    public:

    };

    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_pay_client, KFPayClientInterface );
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif