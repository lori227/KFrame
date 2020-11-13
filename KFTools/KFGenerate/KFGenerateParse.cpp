#include "KFGenerateParse.h"
#include "KFGenerateLogic.h"

namespace KFrame
{
    bool KFGenerateParse::ParseExcel( const char* file )
    {
        KFExcelFile excelfile;
        if ( !excelfile.Open( file ) )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查是否存在", file );
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
    // 第3行 cpp字段类型
    // 第4行 csharp字段类型
    // 第5行 描述( 不需要保存 )
    // 第6行 字段名
#define __OPTION_DATA_LINE__ 1
#define __FILE_FLAG_LINE__ 2
#define __CPP_CLASS_LINE__ 3
#define __CSHARP_CLASS_LINE__ 4
#define __COMMENTS_LINE__ 5
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

        // 保存配置表
        if ( !WriteConfigFile( &exceldata ) )
        {
            return false;
        }

        // 保存生成的配置表
        if ( !WriteCodeFile( &exceldata ) )
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
            auto kfcpp = sheet->FindCell( __CPP_CLASS_LINE__, i );
            if ( kfcpp != nullptr && !kfcpp->_value.empty() )
            {
                attribute._cpp_class = kfcpp->_value;
            }

            // 读取客户端类型
            auto kfsharp = sheet->FindCell( __CSHARP_CLASS_LINE__, i );
            if ( kfsharp != nullptr && !kfsharp->_value.empty() )
            {
                attribute._csharp_class = kfsharp->_value;
            }

            // 注释
            auto kfcomments = sheet->FindCell( __COMMENTS_LINE__, i );
            if ( kfcomments != nullptr && !kfcomments->_value.empty() )
            {
                attribute._comments = kfcomments->_value;
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

    bool KFGenerateParse::WriteConfigFile( ExcelFileData* exceldata )
    {
        // xml
        {
            auto option = exceldata->FindOption( "xml" );
            if ( option != nullptr )
            {
                if ( !WriteXmlFile( exceldata, option->_value ) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool KFGenerateParse::WriteXmlFile( ExcelFileData* exceldata, std::string filename )
    {
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        auto file = _logic->_server_xml_path + "/" + filename + ".xml";

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查目录是否正确", file );
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

    bool KFGenerateParse::WriteCodeFile( ExcelFileData* exceldata )
    {
        // cpp
        {
            auto option = exceldata->FindOption( "cpp" );
            if ( option != nullptr )
            {
                if ( !WriteCppFile( exceldata, option->_value ) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool KFGenerateParse::GenreateCppFile( ExcelFileData* exceldata, std::string filename )
    {
        // 检查cpp的类数据
        std::map< std::string, CodeClass > _class_list;
        CheckCppClassData( exceldata, _class_list );

        // 开始写文件
        return WriteCppFile( exceldata, _class_list, filename );
    }

    void KFGenerateParse::CheckCppClassData( ExcelFileData* exceldata, std::map< std::string, CodeClass >& classlist )
    {
        classlist.clear();
        for ( auto& iter : exceldata->_attributes )
        {
            auto attribute = &iter.second;

            // 把配置表字段转换成cpp变量
            attribute->_cpp_name = TransformNameToCpp( attribute->_name );

            // 检查父类
            FormatCppParentClass( attribute, classlist );
        }
    }

    std::string KFGenerateParse::TransformNameToCpp( const std::string& name )
    {
        std::string cppname;
        for ( auto ch : name )
        {
            if ( ch >= 'A' && ch <= 'Z' )
            {
                cppname.append( "_" );
            }

            cppname.append( 1, ch );
        }

        std::transform( cppname.begin(), cppname.end(), cppname.begin(), ::tolower );
        return cppname;
    }

    void KFGenerateParse::FormatCppParentClass( ExcelAttribute* attribute, std::map<std::string, CodeClass>& classlist )
    {
        auto cppclass = attribute->_cpp_class;
        auto classname = KFUtility::SplitString( cppclass, "->" );
        auto parentclassname = KFUtility::SplitString( cppclass, "->" );
        if ( !parentclassname.empty() )
        {
            attribute->_cpp_class = classname;
            attribute->_cpp_parent_class = parentclassname;

            auto iter = classlist.find( parentclassname );
            if ( iter == classlist.end() )
            {
                iter = classlist.insert( std::make_pair( parentclassname, CodeClass() ) ).first;
                iter->second._name = parentclassname;
            }

            iter->second._columns.push_back( attribute->_column );
        }
    }


#define __WRITE_CPP_ATTRIBUTE__( attribute ) \
    if ( !attribute->_comments.empty() )\
    {\
        xmlfile << __FORMAT__( "\t\t// {}\n", attribute->_comments );\
    }\
    auto typeinfo = _logic->_type_list.Find( attribute->_cpp_class );\
    if ( typeinfo == nullptr )\
    {\
        _error = __FORMAT__( "找不到[{}]的类型配置, 请在_generate.xml添加!", attribute->_cpp_class );\
        return false;\
    }\
    if ( typeinfo->_cpp_name == "uint32" )\
    {\
        xmlfile << __FORMAT__( "\t\t{} {} = 0u;\n", typeinfo->_cpp_name, attribute->_cpp_name );\
    }\
    else\
    {\
        xmlfile << __FORMAT__( "\t\t{} {};\n", typeinfo->_cpp_name, attribute->_cpp_name );\
    }\

    bool KFGenerateParse::WriteCppFile( ExcelFileData* exceldata, std::map<std::string, CodeClass>& classlist, std::string filename )
    {
        auto configname = __FORMAT__( "KF{}Config", filename );
        auto file = _logic->_cpp_file_path + "/" + configname + ".hpp";

        auto keyoption = exceldata->FindOption( "key" );
        if ( keyoption == nullptr )
        {
            _error = __FORMAT__( "找不到key对应的配置项" );
            return false;
        }

        auto stroption = keyoption->_value;
        auto keyname = KFUtility::SplitString( stroption, __SPLIT_STRING__ );
        auto settingsubname = KFUtility::SplitString( stroption, __SPLIT_STRING__ );
        if ( settingsubname != "Int" && settingsubname != "Str" )
        {
            _error = __FORMAT__( "继承的配置字段[{}]错误", settingsubname );
            return false;
        }

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查目录是否正确", file );
            return false;
        }

        auto upname = filename;
        std::transform( upname.begin(), upname.end(), upname.begin(), ::toupper );

        xmlfile << __FORMAT__( "#ifndef	__KF_{}_CONFIG_H__\n", upname );
        xmlfile << __FORMAT__( "#define	__KF_{}_CONFIG_H__\n", upname );
        xmlfile << "namespace KFrame\n";
        xmlfile << "{\n";
        xmlfile << "\t/////////////////////////////////////////////////////////////////////////////////\n";

        // 先写子类
        for ( auto& iter : classlist )
        {
            auto codeclass = &iter.second;
            xmlfile << __FORMAT__( "\tclass {}\n", codeclass->_name );
            xmlfile << "\t{\n";
            xmlfile << "\tpublic:\n";

            // 子属性
            for ( auto column : codeclass->_columns )
            {
                auto attribute = exceldata->FindAttribute( column );
                if ( attribute == nullptr )
                {
                    continue;
                }

                // 基础属性
                __WRITE_CPP_ATTRIBUTE__( attribute );
            }

            xmlfile << "\t};\n\n";
        }
        xmlfile << "\t/////////////////////////////////////////////////////////////////////////////////\n";

        // 写配置类
        xmlfile << __FORMAT__( "\tclass KF{}Setting : public KF{}Setting\n", filename, settingsubname );
        xmlfile << "\t{\n";
        xmlfile << "\tpublic:\n";
        for ( auto& iter : exceldata->_attributes )
        {
            auto attribute = &iter.second;
            if ( !attribute->_cpp_parent_class.empty() )
            {
                continue;
            }

            // 基础属性
            __WRITE_CPP_ATTRIBUTE__( attribute );

            // 写类

        }

        xmlfile << "\t};\n\n";

        xmlfile << "}\n";
        xmlfile << "#endif\n";
    }
}
