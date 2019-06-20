#include "KFParse.h"
#include "Utf8ToGbk.hpp"
#include "KFUtility/KFUtility.h"
#include "KFUtility/KFDate.h"
#include "KFrame.h"

namespace KFrame
{
    KFFile* KFParse::AddFile( std::string& name )
    {
        return &_files[ name ];
    }

    bool KFParse::LoadFromExcel( const char* file, bool utf8 )
    {
        _utf8 = utf8;

        KFExcelFile excelfile;
        if ( !excelfile.Open( file ) )
        {
            std::cout << "can't open " << file << std::endl;
            return false;
        }

        for ( auto sheet : excelfile._sheets )
        {
            LoadFromExcel( sheet );
        }
        return true;
    }

    bool KFParse::LoadFromExcel( KFExcelSheet* sheet )
    {
        // 第1行 第2列为文件名
        auto kfname = sheet->FindCell( 1, 2 );
        if ( kfname == nullptr )
        {
            return false;
        }

        auto name = kfname->_value;
        KFUtility::SplitString( name, "#" );
        if ( name.empty() )
        {
            return false;
        }

        auto kffile = AddFile( name );
        kffile->_class._class_name = name;

        // 第2行字段描述, 不读
        // 第3行字段保存属性 1 服务器 2 客户端 3所有
        // 第4行字段名
        static auto _begin_row = 4;
        static auto _begin_col = 1;
        std::unordered_map< int32, std::string > _names;
        for ( auto i = _begin_col; i <= sheet->_dimension._last_col; ++i )
        {
            auto kfname = sheet->FindCell( _begin_row, i );
            if ( kfname == nullptr || kfname->_value.empty() )
            {
                continue;
            }


            KFUtility::ReplaceString( kfname->_value, " ", "" );

            auto strname = kfname->_value;

            KFAttribute attribute;
            attribute._name = KFUtility::SplitString( strname, "(" );
            attribute._type = KFUtility::SplitString( strname, ")" );
            std::transform( attribute._type.begin(), attribute._type.end(), attribute._type.begin(), ::tolower );

            auto kfsave = sheet->FindCell( _begin_row - 1, i );
            if ( kfsave != nullptr && !kfsave->_value.empty() )
            {
                if ( kfsave->_value == "1" )
                {
                    attribute._is_server = true;
                    attribute._is_client = false;
                }
                else if ( kfsave->_value == "2" )
                {
                    attribute._is_server = false;
                    attribute._is_client = true;
                }
                else if ( kfsave->_value == "3" )
                {
                    attribute._is_server = true;
                    attribute._is_client = true;
                }
                else
                {
                    attribute._is_client = false;
                    attribute._is_server = false;
                }
            }

            kffile->_class.AddAttribute( i, attribute );
        }

        // 第2行, 开始读取配置
        static auto _data_row = _begin_row + 1;
        for ( auto i = _data_row; i <= sheet->_dimension._last_row; ++i )
        {
            KFData data;
            for ( auto j = _begin_col; j <= sheet->_dimension._last_col; ++j )
            {
                auto attribute = kffile->_class.GetAttribute( j );
                if ( attribute == nullptr )
                {
                    continue;
                }

                std::string value = "";
                auto kfvalue = sheet->FindCell( i, j );
                if ( kfvalue != nullptr )
                {
                    value = kfvalue->_value;
                }

                if ( attribute->_name == "Id" && value.empty() )
                {
                    break;
                }

                KFUtility::ReplaceString( value, "\"", "&quot;" );
                data.AddData( j, value );
            }

            kffile->AddData( data );
        }

        return true;
    }

    bool KFParse::SaveToHpp( const char* file )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToHpp( file, &iter.second ) )
            {
                return false;
            }
        }
        /*std::string content = "";

        content += "#ifndef __KF_FIELD_H__\n";
        content += "#define __KF_FIELD_H__\n";
        content += "#include \"KFInclude.h\"\n";
        content += "\n";
        content += "namespace KFrame\n";
        content += "{\n";
        content += "\tnamespace KFField\n";
        content += "\t{\n";

        for ( auto& iter : _kf_field_manage->_field_data_list._objects )
        {
        	auto fielddata = iter.second;

        	content += KFUtility::Format( "\t\tstatic const std::string& _{} = \"{}\";\n",
        		fielddata->_name.c_str(), fielddata->_value.c_str() );
        }

        content += "\n";
        content += "\t}\n";
        content += "}\n";
        content += "\n";
        content += "#endif\n";

        std::ofstream cppfile( file );
        if ( !cppfile )
        {
        	std::cout << "Can't Open hpp = " << file << std::endl;
        	return false;
        }

        cppfile << content;
        cppfile.flush();
        cppfile.close();
        return true;*/

        return true;
    }

    bool KFParse::SaveToHpp( const std::string& path, KFFile* kffile )
    {
        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::toupper );
        std::transform( filename.begin() + 1, filename.end(), filename.begin() + 1, ::tolower );
        auto file = path + "/KF" + filename + ".h";

        std::ofstream hppfile( file );
        if ( !hppfile )
        {
            std::cout << "Can't Open hpp = " << file << std::endl;
            return false;
        }

        std::string upfilename = filename;
        std::transform( filename.begin(), filename.end(), upfilename.begin(), ::toupper );
        hppfile << "#ifndef __KF_" << upfilename << "_CONFIG_H__\n";
        hppfile << "#ifndef __KF_" << upfilename << "_CONFIG_H__\n";
        hppfile << "\n";
        hppfile << "#include \"KFrame.h\"\n";
        hppfile << "#include \"KFConfig/KFConfigInterface.h\"\n";
        hppfile << "\n";
        hppfile << "namespace KFrame\n";
        hppfile << "{\n";

        ////////////////////////////////////////////////////////////////////////////////////

        hppfile << "\tclass KF" << filename << "Setting\n";
        hppfile << "\t{\n";
        hppfile << "\tpublic:\n";

        for ( auto iter : kffile->_class._attributes )
        {
            auto attribute = &iter.second;

            hppfile << "\t\t// " << attribute->_comment << "\n";
            std::string strname = changeString( attribute->_name );

            if ( "string" == attribute->_type )
            {
                hppfile << "\t\tstd::" << attribute->_type << " " << strname << ";\n";
            }
            else if ( "uint32" == attribute->_type )
            {
                hppfile << "\t\t" << attribute->_type << " " << strname << ";\n";
            }
            else
            {
                std::cout << "参数类型错误：[" << attribute->_type << "] " << attribute->_name << std::endl;
                system( "pause" );
            }
            hppfile << "\n";
        }
        hppfile << "\t};\n";
        hppfile << "\n";
        hppfile << "\n";

        ////////////////////////////////////////////////////////////////////////////////////

        hppfile << "\tclass KF" << filename << "Config : public KFConfig, public KFSingleton< KF" << filename << "Config >\n";
        hppfile << "\t{\n";
        hppfile << "\tpublic:\n";
        hppfile << "\t\tKF" << filename << "Config();\n";
        hppfile << "\t\t~KF" << filename << "Config();\n";
        hppfile << "\n";
        hppfile << "\t\tbool LoadConfig();\n";
        hppfile << "\n";

        //判断key的类型
        auto typeiter = kffile->_class._attributes.begin();
        if ( "string" == typeiter->second._type )
        {
            hppfile << "\t\tconst KF" << filename << "Setting* Get" << filename << "Setting( const std::string& key );\n";
        }
        else if ( "uint32" == typeiter->second._type )
        {
            hppfile << "\t\tconst KF" << filename << "Setting* Get" << filename << "Setting( uint32 key );\n";
        }
        else
        {
            std::cout << "参数类型错误：[" << typeiter->second._type << "] " << typeiter->second._name << std::endl;
            system( "pause" );
        }

        hppfile << "\n";
        hppfile << "\tpublic:\n";

        std::string lowfilename = filename;
        std::transform( filename.begin(), filename.end(), lowfilename.begin(), ::tolower );
        if ( "string" == typeiter->second._type )
        {
            hppfile << "\t\tstd::map<std::string, KF" << filename << "Setting > _kf_" << lowfilename << "_setting;\n";
        }
        else if ( "uint32" == typeiter->second._type )
        {
            hppfile << "\t\tstd::map<uint32, KF" << filename << "Setting > _kf_" << lowfilename << "_setting;\n";
        }
        else
        {
            std::cout << "参数类型错误：[" << typeiter->second._type << "] " << typeiter->second._name << std::endl;
            system( "pause" );
        }

        hppfile << "\t};\n";
        hppfile << "\n";

        hppfile << "\tstatic auto _kf_" << lowfilename << "_config = KF" << filename << "Config::Instance();\n";
        hppfile << "\n";

        hppfile << "}\n";
        hppfile << "\n";
        hppfile << "#endif\n";

        hppfile.flush();
        hppfile.close();
        return true;
    }

    bool KFParse::SaveToCpp( const char* file )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToCpp( file, &iter.second ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToCpp( const std::string& path, KFFile* kffile )
    {
        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::toupper );
        std::transform( filename.begin() + 1, filename.end(), filename.begin() + 1, ::tolower );
        auto file = path + "/KF" + filename + ".cpp";

        std::ofstream cppfile( file );
        if ( !cppfile )
        {
            std::cout << "Can't Open cpp = " << file << std::endl;
            return false;
        }

        cppfile << "#include \"KF" << filename << "Config.h\"\n";
        cppfile << "\n";

        cppfile << "namespace KFrame\n";
        cppfile << "{\n";
        cppfile << "\n";

        cppfile << "\tKF" << filename << "Config::KF" << filename << "Config()\n";
        cppfile << "\t{\n";
        cppfile << "\n";
        cppfile << "\t}\n";
        cppfile << "\n";

        cppfile << "\tKF" << filename << "Config::~KF" << filename << "Config()\n";
        cppfile << "\t{\n";
        cppfile << "\n";
        cppfile << "\t}\n";
        cppfile << "\n";
        cppfile << "\n";

        //LoadConfig函数
        cppfile << "\tbool KF" << filename << "Config::LoadConfig()\n";
        cppfile << "\t{\n";
        cppfile << "\t\t" << "try" << "\n";
        cppfile << "\t\t" << "{" << "\n";

        cppfile << "\t\t\t" << "KFXml kfxml( file );" << "\n";
        cppfile << "\t\t\t" << "auto config = kfxml.RootNode();" << "\n";
        cppfile << "\t\t\t" << "auto node = config.FindNode( \"Setting\" );" << "\n";
        cppfile << "\t\t\t" << "while ( node.IsValid() )" << "\n";
        cppfile << "\t\t\t" << "{" << "\n";
        cppfile << "\t\t\t\t" << "KF" << filename << "Setting setting;" << "\n";

        for ( auto iter : kffile->_class._attributes )
        {
            auto attribute = &iter.second;
            std::string strname = changeString( attribute->_name );
            if ( "string" == attribute->_type )
            {
                cppfile << "\t\t\t\t" << "setting." << strname << " = node.GetString( \"" << attribute->_name << "\" );" << "\n";
            }
            else if ( "uint32" == attribute->_type )
            {
                cppfile << "\t\t\t\t" << "setting." << strname << " = node.GetUInt32( \"" << attribute->_name << "\" );" << "\n";
            }
            else
            {
                std::cout << "参数类型错误：[" << attribute->_type << "] " << attribute->_name << std::endl;
                system( "pause" );
            }
        }
        cppfile << "\n";

        //获取key的变量名
        auto attiter = kffile->_class._attributes.begin();
        std::string keyname = changeString( attiter->second._name );

        std::string lowfilename = filename;
        std::transform( filename.begin(), filename.end(), lowfilename.begin(), ::tolower );
        cppfile << "\t\t\t\t" << "_kf_" << lowfilename << "_setting.insert( std::make_pair( setting." << keyname << ", setting ) );" << "\n";
        cppfile << "\t\t\t\t" << "node.NextNode();" << "\n";

        cppfile << "\t\t\t" << "}" << "\n";
        cppfile << "\t\t" << "}" << "\n";
        cppfile << "\t\t" << "catch ( ... )" << "\n";
        cppfile << "\t\t" << "{" << "\n";
        cppfile << "\t\t\t" << "return false;" << "\n";
        cppfile << "\t\t" << "}" << "\n";
        cppfile << "\t\t" << "return true;" << "\n";
        cppfile << "\t}\n";
        cppfile << "\n";

        //Get函数
        if ( "string" == attiter->second._type )
        {
            cppfile << "\tconst KF" << filename << "Setting* KF" << filename << "Config::Get" << filename << "Setting( const std::string& key )\n";
        }
        else if ( "uint32" == attiter->second._type )
        {
            cppfile << "\tconst KF" << filename << "Setting* KF" << filename << "Config::Get" << filename << "Setting( uint32 key )\n";
        }
        else
        {
            std::cout << "参数类型错误：[" << attiter->second._type << "] " << attiter->second._name << std::endl;
            system( "pause" );
        }
        cppfile << "\t{\n";
        cppfile << "\t\t" << "auto iter = _kf_" << lowfilename << "_setting.find( key );" << "\n";
        cppfile << "\t\t" << "if ( iter == _kf_" << lowfilename << "_setting.end() )" << "\n";
        cppfile << "\t\t" << "{" << "\n";
        cppfile << "\t\t\t" << "return nullptr;" << "\n";
        cppfile << "\t\t" << "}" << "\n";
        cppfile << "\t\t" << "return &iter->second;" << "\n";
        cppfile << "\t}\n";

        cppfile << "}\n";

        cppfile.flush();
        cppfile.close();
        return true;
    }

    bool KFParse::SaveToXml( const char* path )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToXml( path, &iter.second ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToXml( const std::string& path, KFFile* kffile )
    {
        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        auto file = path + "/" + filename + ".xml";

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            std::cout << "Can't Open Xml = " << file << std::endl;
            return false;
        }


        //xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << "<root version=\"00000\">\n";

        for ( auto& iter : kffile->_datas )
        {
            auto kfdata = &iter;
            if ( !kfdata->IsValid() )
            {
                continue;
            }

            xmlfile << "\t<item";

            for ( auto& miter : kfdata->_datas )
            {
                auto attribute = kffile->_class.GetAttribute( miter.first );
                if ( attribute->_is_server )
                {
                    xmlfile << __FORMAT__( " {}=\"{}\"", attribute->_name, miter.second );
                }
            }

            xmlfile << "/>\n";
        }
        xmlfile << "</root>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }

    bool KFParse::SaveToCSV( const char* path )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToCSV( path, &iter.second ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToCSV( const std::string& path, KFFile* kffile )
    {
        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        auto file = path + "/" + filename + ".dat";

        std::ofstream csvfile( file );
        if ( !csvfile )
        {
            std::cout << "Can't Open CSV = " << file << std::endl;
            return false;
        }

        std::string nameline = "";
        std::string typeline = "";
        std::string commentline = "";
        for ( auto iter : kffile->_class._attributes )
        {
            auto attribute = &iter.second;

            nameline += attribute->_name + " ";
            typeline += attribute->_type + " ";
        }

        csvfile << nameline << "\n";
        //csvfile << typeline << "\n";

        for ( auto& iter : kffile->_datas )
        {
            auto kfdata = &iter;

            std::string dataline = "";
            for ( auto& miter : kfdata->_datas )
            {
                dataline += miter.second + " ";
            }

            csvfile << dataline << "\n";
        }

        csvfile.flush();
        csvfile.close();
        return true;
    }

    bool KFParse::SaveToCSharp( const char* path )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToCSharp( path, &iter.second ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToCSharp( const std::string& path, KFFile* kffile )
    {
        {
            return true;
        }

        auto file = path + "/" + kffile->_class._class_name + "Config.cs";
        std::ofstream csharpfile( file );
        if ( !csharpfile )
        {
            std::cout << "Can't Open CSharp = " << file << std::endl;
            return false;
        }

        csharpfile << "// This File Is Generated By Parse Tool, Do Not Edit It!\n";
        csharpfile << "// Generated Time [" << KFDate::GetTimeString() << "]\n";

        csharpfile << "using System;\n";
        csharpfile << "using System.Data;\n";
        csharpfile << "using System.Collections;\n";
        csharpfile << "using System.Collections.Generic;\n";
        csharpfile << "\n";

        csharpfile << "namespace Config\n";
        csharpfile << "{\n";

        // 写类定义
        csharpfile << "\tpublic class " << kffile->_class._class_name << "Setting\n";
        csharpfile << "\t{\n";

        for ( auto iter : kffile->_class._attributes )
        {
            auto attribute = &iter.second;

            csharpfile << "\t\t// " << attribute->_comment << "\n";
            csharpfile << "\t\tpublic " << attribute->_type << " " << attribute->_name << ";\n";
            csharpfile << "\n";
        }
        csharpfile << "\t}\n";
        csharpfile << "\n";
        csharpfile << "\n";

        // 写读取类
        csharpfile << "\t" << __FORMAT__( "public class {}Config : IConfig<{}Setting>, ISingleton<{}Config>\n",
                                          kffile->_class._class_name, kffile->_class._class_name, kffile->_class._class_name );
        csharpfile << "\t{\n";

        // load
        csharpfile << "\t\t" << "public override bool LoadConfig()" << "\n";
        csharpfile << "\t\t" << "{" << "\n";
        csharpfile << "\t\t\t" << "var configreader = new FileReader();" << "\n";
        csharpfile << "\t\t\t" << "var datareader = new DataReader();" << "\n";

        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        csharpfile << "\t\t\t" << __FORMAT__( "var datatable = configreader.ReadFile(IConfig._path + \"{}.csv\");", filename );
        csharpfile << "\n";
        csharpfile << "\t\t\t" << "if(datatable == null)" << "\n";
        csharpfile << "\t\t\t" << "{" << "\n";
        csharpfile << "\t\t\t\t" << "return false;" << "\n";
        csharpfile << "\t\t\t" << "}" << "\n";
        csharpfile << "\n";

        csharpfile << "\t\t\t" << "foreach(var row in datatable.Rows)" << "\n";
        csharpfile << "\t\t\t" << "{" << "\n";
        csharpfile << "\t\t\t\t" << __FORMAT__( "var setting = new {}Setting();", kffile->_class._class_name );
        csharpfile << "\n";

        for ( auto iter : kffile->_class._attributes )
        {
            auto attribute = &iter.second;

            auto totaltype = attribute->_type;
            auto maintype = KFUtility::SplitString( totaltype, "<" );

            std::string subtype = "";
            if ( !totaltype.empty() )
            {
                subtype = "<";
                subtype += totaltype;
            }

            csharpfile << "\t\t\t\t";
            if ( subtype.empty() )
            {
                csharpfile << __FORMAT__( "setting.{} = datareader.GetValue<{}>(row[\"{}\"].ToString());",
                                          attribute->_name, maintype, attribute->_name );
            }
            else
            {
                csharpfile << __FORMAT__( "setting.{} = datareader.Get{}{}(row[\"{}\"].ToString());",
                                          attribute->_name, maintype, subtype, attribute->_name );
            }
            csharpfile << "\n";
        }

        csharpfile << "\t\t\t\t";
        csharpfile << __FORMAT__( "_settings[setting.{}] = setting;", kffile->_class._attributes[ 1 ]._name );
        csharpfile << "\n";

        csharpfile << "\t\t\t" << "}" << "\n";
        csharpfile << "\n";

        csharpfile << "\t\t\t" << "return true;" << "\n";
        csharpfile << "\t\t" << "}" << "\n";

        csharpfile << "\t}\n";
        csharpfile << "\n";
        csharpfile << "}\n";

        return true;
    }

    std::string KFParse::changeString( std::string& oldstring )
    {
        std::string newstring;
        for ( auto iter : oldstring )
        {
            if ( iter >= 'A' && iter <= 'Z' )
            {
                newstring.push_back( '_' );
                newstring.push_back( iter );
            }
            else
            {
                newstring.push_back( iter );
            }
        }

        std::transform( newstring.begin(), newstring.end(), newstring.begin(), ::tolower );
        return newstring;
    }
}