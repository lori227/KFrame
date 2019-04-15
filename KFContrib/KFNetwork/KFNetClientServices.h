#ifndef __NET_CLIENT_SERVICES_H__
#define __NET_CLIENT_SERVICES_H__

#include "KFrame.h"
#include "uv.h"
#include "KFNetServices.h"
#include "KFNetClient.h"

namespace KFrame
{
    class KFNetEvent;
    class KFNetClientServices : public KFNetServices
    {
    public:

        KFNetClientServices();
        ~KFNetClientServices();
    };
}

#endif