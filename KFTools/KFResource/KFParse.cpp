#include "KFParse.h"
#include "KFUtility/KFUtility.h"
#include "KFUtility/KFDate.h"
#include "KFrame.h"

namespace KFrame
{
    KFFile* KFParse::AddFile( std::string& name )
    {
        return &_files[ name ];
    }

    bool KFParse::LoadFromExcel( const char* file, bool utf8, uint32 saveflag )
    {
        _utf8 = utf8;
        _files.clear();

        KFExcelFile excelfile;
        if ( !excelfile.Open( file ) )
        {
            _error = __FORMAT__( "can't open {}", file );
            return false;
        }

        for ( auto sheet : excelfile._sheets )
        {
            auto ok = LoadFromExcel( sheet, saveflag );
            if ( !ok )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::LoadFromExcel( KFExcelSheet* sheet, uint32 saveflag )
    {
        // 第1行 第2列为文件名
        auto kfname = sheet->FindCell( 1, 2 );
        if ( kfname == nullptr )
        {
            _error = __FORMAT__( "{} format error", KFConvert::ToAscii( sheet->_name ) );
            return false;
        }

        auto name = kfname->_value;
        KFUtility::SplitString( name, "#" );
        if ( name.empty() )
        {
            _error = __FORMAT__( "{} no config name", KFConvert::ToAscii( sheet->_name ) );
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

            // saveflag
            auto kfsave = sheet->FindCell( _begin_row - 1, i );
            if ( kfsave != nullptr && !kfsave->_value.empty() )
            {
                attribute._flag = KFUtility::ToValue< uint32 >( kfsave->_value );
            }
            else
            {
                _error = __FORMAT__( "{} field=[{}] savefile empty", sheet->_name, kfname->_value );
                return false;
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
                if ( attribute == nullptr || !KFUtility::HaveBitMask( attribute->_flag, saveflag ) )
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

            if ( data.IsValid() )
            {
                kffile->AddData( data );
            }
        }

        return true;
    }

    bool KFParse::SaveToXml( const char* path, const std::string& version )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToXml( path, &iter.second, version ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToXml( const std::string& path, KFFile* kffile, const std::string& version )
    {
        std::string filename = kffile->_class._class_name;
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        auto file = path + "/" + filename + ".xml";

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "can't open xml=[{}]", filename );
            return false;
        }

        //xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<root version=\"{}\">\n", version );

        for ( auto& iter : kffile->_datas )
        {
            auto kfdata = &iter;
            xmlfile << "\t<item";

            for ( auto& miter : kfdata->_datas )
            {
                auto& strvalue = miter.second;
                if ( !strvalue.empty() && strvalue != "0" )
                {
                    auto attribute = kffile->_class.GetAttribute( miter.first );
                    xmlfile << __FORMAT__( " {}=\"{}\"", attribute->_name, strvalue );
                }
            }

            xmlfile << "/>\n";
        }
        xmlfile << "</root>\n";

        xmlfile.flush();
        xmlfile.close();
        return true;
    }

    bool KFParse::SaveToCSV( const char* path, const std::string& version )
    {
        for ( auto iter : _files )
        {
            if ( !SaveToCSV( path, &iter.second, version ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFParse::SaveToCSV( const std::string& path, KFFile* kffile, const std::string& version )
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
}