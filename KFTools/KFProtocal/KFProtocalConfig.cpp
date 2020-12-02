#include "KFProtocalConfig.h"

namespace KFrame
{
    const std::string KFProtocalConfig::_setting_file = "_zconfig.xml";

    bool KFProtocalConfig::LoadFile( const std::string& file )
    {
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Config" );
            if ( root.IsValid() )
            {
                auto pathnode = root.FindNode( "Path" );
                _server_cpp_path = pathnode.ReadString( "ServerCpp" );
                _server_lua_path = pathnode.ReadString( "ServerLua" );
                _client_cpp_path = pathnode.ReadString( "ClientCpp" );
                _client_lua_path = pathnode.ReadString( "ClientLua" );
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    bool KFProtocalConfig::SaveFile( const std::string& file )
    {
        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            std::cout << "Can't Open Xml = " << file << std::endl;
            return false;
        }

        xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << "<Config>\n";

        xmlfile << __FORMAT__( "\t<Path ServerCpp=\"{}\" ServerLua=\"{}\" ClientCpp=\"{}\" ClientLua=\"{}\"/>\n",
                               _server_cpp_path, _server_lua_path, _client_cpp_path, _client_lua_path );

        xmlfile << "</Config>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }
}
