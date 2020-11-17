#ifndef __KF_PROJECT_CONFIG_H__
#define __KF_PROJECT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFProjectConfig : public KFConfig, public KFInstance< KFProjectConfig >
    {
    public:
        KFProjectConfig()
        {
            _file_name = "project";
        }

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

    public:
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif