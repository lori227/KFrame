#include "KFResourceConfig.h"
#include "Poco/File.h"

namespace KFrame
{
    KFResourceSetting* KFResourceConfig::FindFileSetting( const std::string& name )
    {
        return _files.Find( name );
    }

    KFServerSetting* KFResourceConfig::FindServerSetting( uint32 id )
    {
        return _servers.Find( id );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    bool KFResourceConfig::LoadFile( const std::string& file )
    {
        _files.Clear();
        _servers.Clear();
        ///////////////////////////////////////////////////////////////////////////////////
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Config" );

            auto projectnode = root.FindNode( "Project" );
            _store_name = projectnode.GetString( "Store" );
            _project_name = projectnode.GetString( "Name" );
            _project_time = projectnode.GetUInt64( "Time" );

            // server
            auto serversnode = root.FindNode( "Servers" );
            if ( serversnode.IsValid() )
            {
                auto servernode = serversnode.FindNode( "Server" );
                while ( servernode.IsValid() )
                {
                    auto id = servernode.GetUInt32( "Id" );
                    auto kfsetting = _servers.Create( id );
                    kfsetting->_id = id;
                    kfsetting->_name = KFConvert::ToAscii( servernode.GetString( "Name" ) );
                    kfsetting->_ip = servernode.GetString( "Ip" );
                    kfsetting->_url = servernode.GetString( "Url" );

                    servernode.NextNode();
                }
            }

            // file
            auto filesnode = root.FindNode( "Files" );
            if ( filesnode.IsValid() )
            {
                auto filenode = filesnode.FindNode( "File" );
                while ( filenode.IsValid() )
                {
                    auto name = KFConvert::ToAscii( filenode.GetString( "Name" ) );
                    auto kfsetting = _files.Create( name );

                    kfsetting->_name = name;
                    kfsetting->_type = filenode.GetUInt32( "Type" );
                    kfsetting->_xml_name = filenode.GetString( "XmlName", true );
                    kfsetting->_last_md5_client = filenode.GetString( "MD5Client", true );
                    kfsetting->_last_md5_server = filenode.GetString( "MD5Server", true );
                    filenode.NextNode();
                }
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    bool KFResourceConfig::SaveFile( const std::string& file )
    {
        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            std::cout << "Can't Open Xml = " << file << std::endl;
            return false;
        }

        xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<Config>\n" );

        xmlfile << __FORMAT__( "\t<Project Name=\"{}\" Time=\"{}\" Store=\"{}\"/>\n", _project_name, _project_time, _store_name );

        xmlfile << "\t<Servers>\n";
        for ( auto& iter : _servers._objects )
        {
            auto kfstting = iter.second;
            xmlfile << "\t\t<Server";
            xmlfile << __FORMAT__( " Id=\"{}\"", kfstting->_id );
            xmlfile << __FORMAT__( " Name=\"{}\"", KFConvert::ToUTF8( kfstting->_name ) );
            xmlfile << __FORMAT__( " Ip=\"{}\"", kfstting->_ip );
            xmlfile << __FORMAT__( " Url=\"{}\"", kfstting->_url );
            xmlfile << "/>\n";
        }
        xmlfile << "\t</Servers>\n";

        xmlfile << "\t<Files>\n";
        for ( auto& iter : _files._objects )
        {
            auto kfstting = iter.second;
            xmlfile << "\t\t<File";
            xmlfile << __FORMAT__( " Name=\"{}\"", KFConvert::ToUTF8( kfstting->_name ) );
            xmlfile << __FORMAT__( " Type=\"{}\"", kfstting->_type );
            xmlfile << __FORMAT__( " XmlName=\"{}\"", kfstting->_xml_name );
            xmlfile << __FORMAT__( " MD5Client=\"{}\"", kfstting->_last_md5_client );
            xmlfile << __FORMAT__( " MD5Server=\"{}\"", kfstting->_last_md5_server );
            xmlfile << "/>\n";
        }
        xmlfile << "\t</Files>\n";

        xmlfile << "</Config>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }

    bool KFResourceConfig::LoadSetting( const std::string& file )
    {
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Config" );
            if ( root.IsValid() )
            {
                auto pathnode = root.FindNode( "Path" );
                _server_path = pathnode.GetString( "Server" );
                _client_path = pathnode.GetString( "Client" );

                auto filenode = root.FindNode( "File" );
                _file_type = filenode.GetUInt32( "Type" );

                auto servernode = root.FindNode( "Server" );
                _server_select_id = servernode.GetUInt32( "Id" );
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    bool KFResourceConfig::SaveSetting( const std::string& file )
    {
        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            std::cout << "Can't Open Xml = " << file << std::endl;
            return false;
        }

        xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << "<Config>\n";

        xmlfile << __FORMAT__( "\t<Path Server=\"{}\" Client=\"{}\"/>\n", _server_path, _client_path );
        xmlfile << __FORMAT__( "\t<File Type=\"{}\"/>\n", _file_type );
        xmlfile << __FORMAT__( "\t<Server Id=\"{}\"/>\n", _server_select_id );

        xmlfile << "</Config>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }
}
