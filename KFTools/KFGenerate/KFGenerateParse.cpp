#include "KFGenerateParse.h"
#include "KFGenerateLogic.h"
#include "KFGenerateEvent.h"
#include "KFGenerateVersion.h"

namespace KFrame
{
#define __MAX_PARSE_THREAD__ 4

    void KFGenerateParse::ParseExcels( StringList& listfile )
    {
        _file_list.swap( listfile );

        // 开启4个线程
        _finish_count = 0u;
        for ( auto i = 0; i < __MAX_PARSE_THREAD__; ++i )
        {
            KFThread::CreateThread( this, &KFGenerateParse::ParseExecelThread, __FILE__, __LINE__ );
        }
    }

    void KFGenerateParse::ParseExecelThread()
    {
        auto ok = false;
        do
        {
            ok = false;
            std::string file;
            {
                KFLocker locker( _kf_mutex );
                if ( !_file_list.empty() )
                {
                    file = _file_list.front();
                    _file_list.pop_front();
                }
            }

            if ( !file.empty() )
            {
                ok = ParseExcel( file );
                if ( ok )
                {
                    auto strok = __FORMAT__( "解析=[{}] 完成", file );
                    _event->AddEvent( EventType::ParseOk, 0, strok );
                }
                else
                {
                    auto strerror = __FORMAT__( "解析=[{}], 错误=[{}]", file, _error );
                    _event->AddEvent( EventType::ParseFailed, 0, strerror );
                }
            }
        } while ( ok );

        ++_finish_count;
        if ( _finish_count >= __MAX_PARSE_THREAD__ )
        {
            auto strfinish = __FORMAT__( "解析文件结束" );
            _event->AddEvent( EventType::ParseFinish, 0, strfinish );
        }
    }

    bool KFGenerateParse::ParseExcel( std::string& file )
    {
        auto filedata = _logic->_file_list.Find( file );
        if ( filedata == nullptr )
        {
            _error = __FORMAT__( "找不到文件=[{}]版本信息", file );
            return false;
        }

        try
        {
            KFExcelFile excelfile;
            if ( !excelfile.Open( file.c_str() ) )
            {
                _error = __FORMAT__( "无法打开文件[{}], 请检查是否存在", file );
                return false;
            }

            for ( auto sheet : excelfile._sheets )
            {
                auto ok = LoadExcel( sheet, filedata->_md5_current );
                if ( !ok )
                {
                    return false;
                }
            }

            return true;
        }
        catch ( ... )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查是否存在", file );
        }

        return false;
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

    bool KFGenerateParse::LoadExcel( KFExcelSheet* sheet, const std::string& version )
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
        if ( !WriteConfigFile( &exceldata, version ) )
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

    bool KFGenerateParse::WriteConfigFile( ExcelFileData* exceldata, const std::string& version )
    {
        // xml
        {
            auto option = exceldata->FindOption( "xml" );
            if ( option != nullptr )
            {
                if ( !WriteXmlFile( exceldata, _logic->_server_xml_path, option->_value, version ) )
                {
                    return false;
                }

                if ( !WriteXmlFile( exceldata, _logic->_client_xml_path, option->_value, version ) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool KFGenerateParse::WriteXmlFile( ExcelFileData* exceldata, std::string filepath, std::string filename, const std::string& version )
    {
        if ( filepath.empty() )
        {
            return true;
        }
        std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
        _version->AddVersion( filepath, filename, version );

        auto file = filepath + "/" + filename + ".xml";
        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查目录是否正确", file );
            return false;
        }

        //xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
        xmlfile << __FORMAT__( "<{} {}=\"{}\">\n", __STRING__( root ), __STRING__( version ), version );

        for ( auto& iter : exceldata->_datas )
        {
            auto rowdata = &iter;
            xmlfile << __FORMAT__( "\t<{} {}=\"{}\"", __STRING__( node ), __STRING__( row ), rowdata->_row );

            for ( auto& miter : rowdata->_columns )
            {
                auto attribute = exceldata->FindAttribute( miter.first );
                xmlfile << __FORMAT__( " {}=\"{}\"", attribute->_config_name, miter.second );
            }

            xmlfile << "/>\n";
        }
        xmlfile << __FORMAT__( "</{}>\n", __STRING__( root ) );

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
                if ( !GenreateCppFile( exceldata, option->_value ) )
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
        xmlfile << __FORMAT__( "\t\t{} {} = 0u;\n\n", typeinfo->_cpp_name, attribute->_cpp_name );\
    }\
    else\
    {\
        xmlfile << __FORMAT__( "\t\t{} {};\n\n", typeinfo->_cpp_name, attribute->_cpp_name );\
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
        auto settingsubname = KFUtility::SplitString( stroption, __SPLIT_STRING__ );
        if ( settingsubname != "Int" && settingsubname != "Str" )
        {
            _error = __FORMAT__( "继承的配置字段[{}]错误", settingsubname );
            return false;
        }

        auto keyname = KFUtility::SplitString( stroption, __SPLIT_STRING__ );
        if ( keyname.empty() )
        {
            keyname = __STRING__( row );
        }
        std::transform( keyname.begin(), keyname.end(), keyname.begin(), ::tolower );

        std::ofstream xmlfile( file );
        if ( !xmlfile )
        {
            _error = __FORMAT__( "无法打开文件[{}], 请检查目录是否正确", file );
            return false;
        }

        auto upname = filename;
        std::transform( upname.begin(), upname.end(), upname.begin(), ::toupper );
        auto lowername = filename;
        std::transform( lowername.begin(), lowername.end(), lowername.begin(), ::tolower );


        xmlfile << __FORMAT__( "#ifndef	__KF_{}_CONFIG_H__\n", upname );
        xmlfile << __FORMAT__( "#define	__KF_{}_CONFIG_H__\n\n", upname );

        // 头文件
        {
            xmlfile << "#include \"KFConfig.h\"\n";
            xmlfile << "#include \"KFElementConfig.h\"\n";
            xmlfile << "\n";
        }

        xmlfile << "namespace KFrame\n";
        xmlfile << "{\n";
        xmlfile << "\t/////////////////////////////////////////////////////////////////////////////////\n";

        // 先写子类
        {
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
        }


        // 写配置类
        {
            xmlfile << __FORMAT__( "\tclass KF{}Setting : public KF{}Setting\n", filename, settingsubname );
            xmlfile << "\t{\n";
            xmlfile << "\tpublic:\n";
            for ( auto& iter : exceldata->_attributes )
            {
                auto attribute = &iter.second;
                if ( !attribute->_cpp_parent_class.empty() || attribute->_config_name == keyname )
                {
                    continue;
                }

                // 基础属性
                __WRITE_CPP_ATTRIBUTE__( attribute );

                // 写类
            }
            xmlfile << "\t};\n\n";
            xmlfile << "\t/////////////////////////////////////////////////////////////////////////////////\n";
        }

        // 写配置config
        {
            xmlfile << __FORMAT__( "\tclass KF{}Config : public KFConfigT< KF{}Setting >, public KFInstance< KF{}Config >\n", filename, filename, filename );
            xmlfile << "\t{\n";
            xmlfile << "\tpublic:\n";
            // 构造函数
            {
                xmlfile << __FORMAT__( "\t\tKF{}Config()\n", filename );
                xmlfile << "\t\t{\n";
                xmlfile << __FORMAT__( "\t\t\t_key_name = \"{}\";\n", keyname );
                xmlfile << __FORMAT__( "\t\t\t_file_name = \"{}\";\n", lowername );
                xmlfile << "\t\t}\n\n";
            }

            // 析构函数
            {
                xmlfile << __FORMAT__( "\t\t~KF{}Config() = default;\n\n", filename );
            }

            // 加载完成函数
            {
                xmlfile << "\t\tvirtual void LoadAllComplete()\n";
                xmlfile << "\t\t{\n";
                xmlfile << "\t\t\tfor ( auto& iter : _settings._objects )\n";
                xmlfile << "\t\t\t{\n";
                xmlfile << "\t\t\t\tauto kfsetting = iter.second;\n";

                for ( auto& iter : exceldata->_attributes )
                {
                    auto attribute = &iter.second;
                    if ( attribute->_cpp_class == "element" )
                    {
                        xmlfile << __FORMAT__( "\t\t\t\tKFElementConfig::Instance()->ParseElement( kfsetting->{}, _file_name, kfsetting->_row );\n", attribute->_cpp_name );
                    }
                }

                xmlfile << "\t\t\t}\n";
                xmlfile << "\t\t}\n\n";
            }

            xmlfile << "\tprotected:\n";

            // 读取配置
            {
                xmlfile << __FORMAT__( "\t\tvirtual void ReadSetting( KFXmlNode& xmlnode, KF{}Setting* kfsetting )\n", filename );
                xmlfile << "\t\t{\n";
                for ( auto& iter : exceldata->_attributes )
                {
                    auto attribte = &iter.second;
                    auto typeinfo = _logic->_type_list.Find( attribte->_cpp_class );
                    if ( typeinfo == nullptr )
                    {
                        _error = __FORMAT__( "找不到类型配置=[{}]", attribte->_cpp_class );
                        return false;
                    }

                    xmlfile << __FORMAT__( "\t\t\tkfsetting->{}{} = xmlnode.{}( \"{}\" );\n", attribte->_cpp_name, typeinfo->_cpp_extend, typeinfo->_cpp_function, attribte->_config_name );
                }
                xmlfile << "\t\t}\n\n";
            }



            xmlfile << "\t};\n\n";
            xmlfile << "\t/////////////////////////////////////////////////////////////////////////////////\n";
        }


        xmlfile << "}\n";
        xmlfile << "#endif\n";

        return true;
    }
}
