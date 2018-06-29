#include "KFPlatformConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFPlatformConfig::KFPlatformConfig()
    {
        _token_expire_time = 0;
        _is_open_activation = false;
        _server_list_type = __TOTAL_SERVER_LIST__;
    }

    KFPlatformConfig::~KFPlatformConfig()
    {

    }

    bool KFPlatformConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto platform = config.FindNode( "Platform" );
            _token_expire_time = platform.GetUInt32( "TokenExpireTime" );
            _is_open_activation = platform.GetBoolen( "OpenActivation" );
            _server_list_type = platform.GetUInt32( "ServerListType" );

        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}