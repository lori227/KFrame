#ifndef __KF_HTTP_URL_H__
#define __KF_HTTP_URL_H__

#include "KFrame.h"

namespace KFrame
{

    class KFHttpURL
    {
    public:
        KFHttpURL() = default;

        static std::string EncodeParams( const std::map<std::string, std::string>& params );
    };

}


#endif