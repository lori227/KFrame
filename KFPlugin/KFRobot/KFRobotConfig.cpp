#include "KFRobotConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    void KFRobotConfig::LoadConfig( const std::string& file )
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        {
            auto xmlnode = config.FindNode( "Auth" );
            _auth_address = xmlnode.GetString( "Address" );
        }

        {
            auto xmlnode = config.FindNode( "Login" );
            _total_robot_count = xmlnode.GetUInt32( "Count" );
            _login_interval_time = xmlnode.GetUInt32( "Interval" );
        }

        {
            auto xmlnode = config.FindNode( "Log" );
            _open_data_log = xmlnode.GetBoolen( "Data" );
        }
        //////////////////////////////////////////////////////////////////
    }
}