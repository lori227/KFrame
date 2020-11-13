#include "KFGenerateParse.h"
#include "KFGenerateLogic.h"

namespace KFrame
{
    bool KFGenerateParse::ParseExcel( const char* file )
    {
        KFExcelFile excelfile;
        if ( !excelfile.Open( file ) )
        {
            _error = __FORMAT__( "can't open {}", file );
            return false;
        }

        for ( auto sheet : excelfile._sheets )
        {
            auto ok = LoadExcel( sheet );
            if ( !ok )
            {
                return false;
            }
        }

        return true;
    }

    // 第1行 option data
    // 第2行 字段标记 FileType
    // 第3行 服务器字段类型
    // 第4行 客户端字段类型
    // 第5行 描述( 不需要保存 )
    // 第6行 字段名
#define __OPTION_DATA_LINE__ 1
#define __FILE_FLAG_LINE__ 2
#define __SERVER_CLASS_LINE__ 3
#define __CLIENT_CLASS_LINE__ 4
#define __FILED_NAME_LINE__ 6

    // 起始列索引
#define __BEGIN_CLOUMN__ 1



    bool KFGenerateParse::LoadExcel( KFExcelSheet* sheet )
    {
        ExcelFileData exceldata;

        // 读取字段属性
        if ( !ReadExcelAttribute( sheet, &exceldata ) )
        {
            return false;
        }

        // 先读取option数据
        ReadExcelOptionData( sheet, &exceldata );
        // 读取配置数据
        ReadExcelContent( sheet, &exceldata );

        // 先保存配置表
        if ( !SaveConfigFile( &exceldata ) )
        {
            return false;
        }

        return true;
    }

    void KFGenerateParse::ReadExcelOptionData( KFExcelSheet* sheet, ExcelFileData* exceldata )
    {
        // 第一行是option数据
        for ( auto i = __BEGIN_CLOUMN__; i <= sheet->_dimension._last_col; ++i )
        {
            auto kfoption = sheet->FindCell( __OPTION_DATA_LINE__, i );
            if ( kfoption == nullptr || kfoption->_value.empty() )
            {
                continue;
            }

            ExcelOption optiondata;
            auto stroption = kfoption->_value;
            optiondata._key = KFUtility::SplitString( stroption, "=" );
            optiondata._value = KFUtility::SplitString( stroption, "=" );
            if ( optiondata.IsValid() )
            {
                exceldata->_options[ optiondata._key ] = optiondata;
            }
        }
    }

    bool KFGenerateParse::ReadExcelAttribute( KFExcelSheet* sheet, ExcelFileData* exceldata )
    {
        for ( auto i = __BEGIN_CLOUMN__; i <= sheet->_dimension._last_col; ++i )
        {
            // 读取字段类型
            auto kfflag = sheet->FindCell( __FILE_FLAG_LINE__, i );
            if ( kfflag == nullptr || kfflag->_value.empty() )
            {
                continue;
            }

            // 不是文件类型标记
            auto filedflag = __TO_UINT32__( kfflag->_value );
            if ( !KFUtility::HaveBitMask( filedflag, _logic->_file_type ) )
            {
                continue;
            }

            // 读取字段名字
            auto kfname = sheet->FindCell( __FILED_NAME_LINE__, i );
            if ( kfname == nullptr || kfname->_value.empty() )
            {
                _error = __FORMAT__( "分页:{} 第{}列 没有设置字段名", KFConvert::ToAscii( sheet->_name ), i );
                return false;
            }

            ExcelAttribute attribute;
            attribute._column = i;
            attribute._flag = filedflag;
            attribute._name = kfname->_value;
            attribute._config_name = kfname->_value;
            std::transform( attribute._config_name.begin(), attribute._config_name.end(), attribute._config_name.begin(), ::tolower );

            // 读取服务器类型
            auto kfserver = sheet->FindCell( __SERVER_CLASS_LINE__, i );
            if ( kfserver != nullptr && !kfserver->_value.empty() )
            {
                attribute._server_class = kfserver->_value;
            }

            // 读取客户端类型
            auto kfclient = sheet->FindCell( __CLIENT_CLASS_LINE__, i );
            if ( kfclient != nullptr && !kfclient->_value.empty() )
            {
                attribute._client_class = kfclient->_value;
            }

            exceldata->_attributes[ attribute._column ] = attribute;
        }

        return true;
    }

    bool KFGenerateParse::ReadExcelContent( KFExcelSheet* sheet, ExcelFileData* exceldata )
    {
        for ( auto i = __FILED_NAME_LINE__ + 1; i <= sheet->_dimension._last_row; ++i )
        {
            ExcelRow rowdata;
            rowdata._row = i;
            for ( auto j = __BEGIN_CLOUMN__; j <= sheet->_dimension._last_col; ++j )
            {
                auto attribute = exceldata->FindAttribute( j );
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

                rowdata.AddData( j, value );
            }

            exceldata->_datas.push_back( rowdata );
        }

        return true;
    }

    bool KFGenerateParse::SaveConfigFile( ExcelFileData* exceldata )
    {
        // xml
        {
            auto option = exceldata->FindOption( "xml" );
            if ( option != nullptr )
            {
                if ( !SaveXmlFile( exceldata, option->_value ) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool KFGenerateParse::SaveXmlFile( ExcelFileData* exceldata, std::string filename )
    {
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        auto file = _logic->_server_xml_path + "/" + filename + ".xml";

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "can't open xml=[{}]", file );
            return false;
        }

        //xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<{} {}=\"{}\">\n", _xml_root_name, _xml_version_name, 0000 );

        for ( auto& iter : exceldata->_datas )
        {
            auto rowdata = &iter;
            xmlfile << __FORMAT__( "\t<{} {}=\"{}\"", _xml_node_name, _xml_row_name, rowdata->_row );

            for ( auto& miter : rowdata->_columns )
            {
                auto attribute = exceldata->FindAttribute( miter.first );
                xmlfile << __FORMAT__( " {}=\"{}\"", attribute->_config_name, miter.second );
            }

            xmlfile << "/>\n";
        }
        xmlfile << __FORMAT__( "</{}>\n", _xml_root_name );

        xmlfile.flush();
        xmlfile.close();

        return true;
    }
}
