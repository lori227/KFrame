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
                    _server_id = generatenode.ReadString( "serverid" );

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
                    filedata->_md5_client_repository = filenode.ReadString( "md5client" );
                    filedata->_md5_server_repository = filenode.ReadString( "md5server" );

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
            xmlfile << __FORMAT__( "\t<file name=\"{}\" type=\"{}\" md5client=\"{}\" md5server=\"{}\"/>\n",
                                   KFConvert::ToUTF8( filedata->_name ), filedata->_type, filedata->_md5_client_repository, filedata->_md5_server_repository );
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
                    auto id = servernode.ReadString( "id" );
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
                    repositorydata->_mail = repositorynode.ReadString( "mail" );
                    repositorydata->_password = repositorynode.ReadString( "password" );
                    repositorydata->_path = repositorynode.ReadString( "path" );

                    repositorynode.NextNode();
                }
            }

            // 提交数据
            {
                auto node = root.FindNode( "commits" );
                _commit_data._merge_message = KFConvert::ToAscii( node.ReadString( "merge" ) );
                _commit_data._push_message = KFConvert::ToAscii( node.ReadString( "push" ) );

                auto commitnode = node.FindNode( "commit" );
                while ( commitnode.IsValid() )
                {
                    auto file = commitnode.ReadString( "file" );
                    _commit_data._commit_file_list.push_back( file );
                    commitnode.NextNode();
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
                    typedata->_cpp_function = typenode.ReadString( "cppfunction" );
                    typedata->_cpp_extend = typenode.ReadString( "cppextend" );
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

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGenerateLogic::RunCheckExcelMd5Thread()
    {
        auto _next_check_time = _invalid_int;
        while ( _thread_run )
        {
            if ( KFGlobal::Instance()->_game_time > _next_check_time )
            {
                _next_check_time = KFGlobal::Instance()->_game_time + 10000;

                CheckAllExcelMd5();
            }

            KFThread::Sleep( 1 );
        }
    }

    std::string KFGenerateLogic::FormatExecelName( const std::string& filename )
    {
        auto temp = filename;

        // 把文件夹去掉
        KFUtility::SplitString( temp, "\\" );
        return temp;
    }


    void KFGenerateLogic::CheckAllExcelMd5()
    {
        // 列出所有的excel文件
        Poco::File file( "./table" );
        std::vector< Poco::File > filelist;
        file.list( filelist );

        bool savexml = false;
        auto filecount = 0u;
        auto nowtime = KFGlobal::Instance()->_game_time;

        // 先检查删除的文件
        for ( auto& file : filelist )
        {
            auto filepath = KFConvert::ToAscii( file.path() );
            auto filename = FormatExecelName( filepath );
            auto ok = IsExcelFile( filename );
            if ( !ok )
            {
                continue;
            }

            ++filecount;
            auto filedata = _file_list.Find( filename );
            if ( filedata == nullptr )
            {
                savexml = true;
                filedata = _file_list.Create( filename );
                filedata->_name = filename;
            }
            if ( filedata->_path.empty() )
            {
                filedata->_path = filepath;
            }

            filedata->_last_md5_check_time = nowtime;
            filedata->_md5_current = KFCrypto::Md5File( filepath );
            if ( KFUtility::HaveBitMask( _file_type, ( uint32 )FileType::Server ) )
            {
                if ( filedata->IsServerFile() && filedata->_md5_server_repository != filedata->_md5_current )
                {
                    _event->AddEvent( EventType::AddFile, filedata->_type, filedata->_name );
                }
            }
            else if ( KFUtility::HaveBitMask( _file_type, ( uint32 )FileType::Client ) )
            {
                if ( filedata->IsClientFile() && filedata->_md5_client_repository != filedata->_md5_current )
                {
                    _event->AddEvent( EventType::AddFile, filedata->_type, filedata->_name );
                }
            }
        }


        if ( filecount != _file_list.Size() )
        {
            // 说明有文件删除了
            savexml = true;
            StringList removes;
            for ( auto& iter : _file_list._objects )
            {
                auto filedata = iter.second;
                if ( filedata->_last_md5_check_time != nowtime )
                {
                    removes.push_back( filedata->_name );
                }
            }

            for ( auto& filename : removes )
            {
                _file_list.Remove( filename );
                _event->AddEvent( EventType::RemoveFile, 0, filename );
            }
        }

        if ( savexml )
        {
            SaveExcelXml();
        }
    }


    bool KFGenerateLogic::IsExcelFile( const std::string& path )
    {
        if ( path.at( 0 ) == '~' )
        {
            return false;
        }

        Poco::Path pocopath( path );
        auto& extension = pocopath.getExtension();
        return extension == "xlsx";
    }
}
