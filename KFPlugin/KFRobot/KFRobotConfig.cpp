#include "KFRobotConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFRobotConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        {
            auto xml_node = config.FindNode( "Auth" );
            _auth_address = xml_node.GetString( "Address" );
        }

        {
            auto xml_node = config.FindNode( "Net" );
            _compress_level = xml_node.GetUInt32( "Compress" );
        }

        {
            auto xml_node = config.FindNode( "Login" );
            _robot_account = xml_node.GetString( "Account" );
            _total_robot_count = xml_node.GetUInt32( "Count" );
            _login_interval_time = xml_node.GetUInt32( "Interval" );
        }

        {
            auto xml_node = config.FindNode( "Log" );
            _open_data_log = xml_node.GetBoolen( "Data" );
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}