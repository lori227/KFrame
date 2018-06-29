#include "KFTcpServerConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFTcpServerConfig::LoadConfig( const char* file )
    {
        std::string temp = "";

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto listen = config.FindNode( "Listen" );

            _kf_listen._local_ip = listen.GetString( "LocalIp" );
            _kf_listen._interanet_ip = listen.GetString( "InteranetIp" );
            _kf_listen._type = listen.GetUInt32( "Type" );
            _kf_listen._port = listen.GetUInt32( "Port" );
            _kf_listen._max_connection = listen.GetUInt32( "MaxConnection" );
            _kf_listen._timeout = listen.GetUInt32( "TimeOut" );
            _kf_listen._max_queue_size = listen.GetUInt32( "MaxQueue" );
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}