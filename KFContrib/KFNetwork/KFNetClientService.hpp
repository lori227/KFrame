#ifndef __NET_CLIENT_SERVICE_H__
#define __NET_CLIENT_SERVICE_H__

#include "KFrame.h"
#include "uv.h"
#include "KFNetService.hpp"
#include "KFNetClient.hpp"

namespace KFrame
{
    class KFNetClientService : public KFNetService
    {
    public:
        KFNetClientService();
        virtual ~KFNetClientService();
    };
}

#endif