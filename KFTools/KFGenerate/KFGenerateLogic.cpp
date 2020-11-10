#include "KFGenerateLogic.h"

namespace KFrame
{
    bool KFGenerateLogic::LoadTempXml()
    {
        try
        {
            KFXml kfxml( _temp_xml );
            auto root = kfxml.FindNode( "config" );
            if ( root.IsValid() )
            {
                {
                    auto pathnode = root.FindNode( "serverxml" );
                    _server_xml_path = pathnode.ReadString( "path" );
                }

                {
                    auto pathnode = root.FindNode( "clientxml" );
                    _client_xml_path = pathnode.ReadString( "path" );
                }

                {
                    auto pathnode = root.FindNode( "cpp" );
                    _cpp_file_path = pathnode.ReadString( "path" );
                }

                {
                    auto pathnode = root.FindNode( "csharp" );
                    _csharp_file_path = pathnode.ReadString( "path" );
                }

                {
                    auto pathnode = root.FindNode( "lua" );
                    _lua_file_path = pathnode.ReadString( "path" );
                }

                {
                    auto generatenode = root.FindNode( "generate" );
                    _file_type = generatenode.ReadUInt32( "type" );
                    _repository_type = generatenode.ReadString( "repository" );
                    _server_id = generatenode.ReadUInt32( "serverid" );

                }
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    bool KFGenerateLogic::SaveTempXml()
    {
        std::ofstream xmlfile( _temp_xml );
        if ( !xmlfile )
        {
            return false;
        }

        xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<config>\n" );

        // 路径地址
        xmlfile << __FORMAT__( "\t<serverxml path=\"{}\"/>\n", _server_xml_path );
        xmlfile << __FORMAT__( "\t<clientxml path=\"{}\"/>\n", _client_xml_path );
        xmlfile << __FORMAT__( "\t<cpp path=\"{}\"/>\n", _cpp_file_path );
        xmlfile << __FORMAT__( "\t<csharp path=\"{}\"/>\n", _csharp_file_path );
        xmlfile << __FORMAT__( "\t<lua path=\"{}\"/>\n", _lua_file_path );

        // 生成配置
        xmlfile << __FORMAT__( "\t<generate type=\"{}\" repository=\"{}\" serverid=\"{}\"/>\n", _file_type, _repository_type, _server_id );

        xmlfile << "</config>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }

    bool KFGenerateLogic::LoadExcelXml()
    {
        try
        {
            KFXml kfxml( _excel_xml );
            auto root = kfxml.FindNode( "config" );
            if ( root.IsValid() )
            {
                auto filenode = root.FindNode( "file" );
                while ( filenode.IsValid() )
                {
                    auto name = KFConvert::ToAscii( filenode.ReadString( "name" ) );
                    auto filedata = _file_list.Create( name );
                    filedata->_name = name;
                    filedata->_type = filenode.ReadUInt32( "type" );
                    filedata->_md5_client = filenode.ReadString( "md5client" );
                    filedata->_md5_server = filenode.ReadString( "md5server" );

                    auto codenode = filenode.FindNode( "code" );
                    while ( codenode.IsValid() )
                    {
                        auto type = codenode.ReadString( "type" );
                        auto md5 = codenode.ReadString( "md5" );
                        filedata->_md5_list[ type ] = md5;
                        codenode.NextNode();
                    }

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


    bool KFGenerateLogic::SaveExcelXml()
    {
        std::ofstream xmlfile( _excel_xml );
        if ( !xmlfile )
        {
            return false;
        }

        xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<config>\n" );

        for ( auto& iter : _file_list._objects )
        {
            auto filedata = iter.second;
            xmlfile << __FORMAT__( "\t<file name=\"{}\" type=\"{}\" md5client=\"{}\" md5server=\"{}\">\n",
                                   KFConvert::ToUTF8( filedata->_name ), filedata->_type, filedata->_md5_client, filedata->_md5_server );

            for ( auto fiter : filedata->_md5_list )
            {
                xmlfile << __FORMAT__( "\t\t<code type=\"{}\" md5=\"{}\"/>\n", fiter.first, fiter.second );
            }

            xmlfile <<  "\t</file>";
        }

        xmlfile << "</config>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }

    bool KFGenerateLogic::LoadGenerateXml()
    {
        try
        {
            KFXml kfxml( _generate_xml );
            auto root = kfxml.FindNode( "config" );
            if ( !root.IsValid() )
            {
                return false;
            }

            // 项目配置
            {
                auto node = root.FindNode( "project" );
                _project_name = node.ReadString( "name" );
                _project_time = node.ReadUInt64( "time" );
            }

            // 服务器列表
            {
                auto node = root.FindNode( "servers" );
                auto servernode = node.FindNode( "server" );
                while ( servernode.IsValid() )
                {
                    auto id = servernode.ReadUInt32( "id" );
                    auto serverdata = _server_list.Create( id );
                    serverdata->_id = id;
                    serverdata->_name = KFConvert::ToAscii( servernode.ReadString( "name" ) );
                    serverdata->_url = servernode.ReadString( "url" );

                    servernode.NextNode();
                }
            }

            // 仓库列表
            {
                auto node = root.FindNode( "repositorys" );
                auto repositorynode = node.FindNode( "repository" );
                while ( repositorynode.IsValid() )
                {
                    auto type = repositorynode.ReadString( "type" );
                    auto repositorydata = _repository_list.Create( type );
                    repositorydata->_type = type;
                    repositorydata->_user = repositorynode.ReadString( "user" );
                    repositorydata->_password = repositorynode.ReadString( "password" );
                    repositorydata->_path = repositorynode.ReadString( "path" );

                    repositorynode.NextNode();
                }
            }

            // 类型列表
            {
                auto node = root.FindNode( "types" );
                auto typenode = node.FindNode( "type" );
                while ( typenode.IsValid() )
                {
                    auto name = typenode.ReadString( "name" );
                    auto typedata = _type_list.Create( name );
                    typedata->_name = name;
                    typedata->_cpp_name = typenode.ReadString( "cpp" );
                    typedata->_csharp_name = typenode.ReadString( "csharp" );
                    typedata->_lua_name = typenode.ReadString( "lua" );

                    typenode.NextNode();
                }
            }

        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}
