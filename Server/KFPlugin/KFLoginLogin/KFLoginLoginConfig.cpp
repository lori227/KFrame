#include "KFLoginLoginConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFLoginLoginConfig::KFLoginLoginConfig()
    {
        _world_server_type = 0;
        _gate_server_type = 0;
    }

    KFLoginLoginConfig::~KFLoginLoginConfig()
    {

    }

    bool KFLoginLoginConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto cluster = config.FindNode( "LoginClient" );
            _world_server_type = cluster.GetUInt32( "WorldServerType" );

            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}