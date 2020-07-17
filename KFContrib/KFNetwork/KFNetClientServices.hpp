#ifndef __NET_CLIENT_SERVICES_H__
#define __NET_CLIENT_SERVICES_H__

#include "KFrame.h"
#include "uv.h"
#include "KFNetServices.hpp"
#include "KFNetClient.hpp"

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