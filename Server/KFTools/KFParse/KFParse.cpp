#include "KFParse.h"
#include "Utf8ToGbk.hpp"
#include "KFUtility/KFUtility.h"
#include "KFTime/KFDate.h"
#include "KFConvert/KFConvert.h"

namespace KFrame
{
	KFParse::KFParse()
	{

	}

	KFParse::~KFParse()
	{

	}

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
		auto kffile = AddFile( sheet->_name );

		auto sheetname = sheet->_name;
	
		kffile->_class._class_name = KFUtility::SplitString( sheetname, "-" );
		if ( sheetname == "Server" )
		{
			kffile->_is_server = true;
		}
		else if ( sheetname == "Client" )
		{
			kffile->_is_client = true;
		}
		else
		{
			kffile->_is_client = true;
			kffile->_is_server = true;
		}

		// 第1行属性名 
		static auto _begin_col = 2;
		static auto _begin_row = 1;
		std::map< int32, std::string > _names;
		for ( auto i = _begin_col; i <= sheet->_dimension._last_col; ++i )
		{
			auto kfname = sheet->FindCell( _begin_row, i );
			//auto kfcomment = sheet->FindCell( 2, i );
			//auto kftype = sheet->FindCell( 3, i );

			if ( kfname == nullptr || kfname->_value.empty() 
				/*				kftype == nullptr || kftype->_value.empty()*/ )
			{
				continue;
			}

			KFUtility::ReplaceString( kfname->_value, " ", "" );
			//KFUtility::ReplaceString( kftype->_value, " ", "" );
			//KFUtility::ReplaceString( kfcomment->_value, " ", "" );

			KFAttribute attribute;
			attribute._name = kfname->_value;
			//attribute._type = kftype->_value;
			//attribute._comment = KFConvert::ToAscii( kfcomment->_value );
			kffile->_class.AddAttribute( i, attribute );
		}
		
		// 第2行, 开始读取配置
		static auto _data_row = _begin_row + 3;
		for ( auto i = _data_row; i <= sheet->_dimension._last_row; ++i )
		{
			auto kfvalue = sheet->FindCell( i, _begin_col );
			if ( kfvalue == nullptr || kfvalue->_value.empty() )
			{
				continue;
			}

			KFData data;
			data.AddData( _begin_col, kfvalue->_value );

			for ( auto j = _begin_col + 1; j <= sheet->_dimension._last_col; ++j )
			{
				auto attribute = kffile->_class.GetAttribute( j );
				if ( attribute == nullptr )
				{
					continue;
				}

				std::string value = "";
				auto kfvalue = sheet->FindCell( i, j );
				if ( nullptr != kfvalue )
				{
					value = kfvalue->_value;
				}

				KFUtility::ReplaceString( value, " ", "" );
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

			content += KFUtility::Format( "\t\tstatic const std::string& _%s = \"%s\";\n", 
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
			else if( "uint32" == attribute->_type )
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
		hppfile << "\t\tbool LoadConfig( const char* file );\n";
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
		else if( "uint32" == typeiter->second._type )
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
		cppfile << "\tbool KF" << filename << "Config::LoadConfig( const char* file )\n";
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
			if ( "string" == attribute->_type)
			{
				cppfile << "\t\t\t\t" << "setting." << strname << " = node.GetString( \"" << attribute->_name << "\" );" << "\n";
			}
			else if ( "uint32" == attribute->_type)
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
		auto file = path + "/" + filename + ".config";

		std::ofstream xmlfile( file );
		if ( !xmlfile )
		{
			std::cout << "Can't Open Xml = " << file << std::endl;
			return false;
		}


		xmlfile << "<?xml version = '1.0' encoding = 'utf-8' ?>\n";
		xmlfile << "<Settings>\n";

		for ( auto& iter : kffile->_datas )
		{
			auto kfdata = &iter;
			xmlfile << KFUtility::Format( "\t<Setting" );

			for ( auto& miter : kfdata->_datas )
			{
				auto name = kffile->_class.GetAttribute( miter.first )->_name;
				xmlfile << KFUtility::Format( " %s=\"%s\"", name.c_str(), miter.second.c_str() );
			}

			xmlfile << "/>\n";
		}
		xmlfile << "</Settings>\n";

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
		if ( !kffile->_is_client )
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
		csharpfile << "\t" << KFUtility::Format( "public class %sConfig : IConfig<%sSetting>, ISingleton<%sConfig>\n",
			kffile->_class._class_name.c_str(), kffile->_class._class_name.c_str(), kffile->_class._class_name.c_str() );
		csharpfile << "\t{\n";

		// load
		csharpfile << "\t\t" << "public override bool LoadConfig()" << "\n";
		csharpfile << "\t\t" << "{" << "\n";
		csharpfile << "\t\t\t" << "var configreader = new FileReader();" << "\n";
		csharpfile << "\t\t\t" << "var datareader = new DataReader();" << "\n";

		std::string filename = kffile->_class._class_name;
		std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
		csharpfile << "\t\t\t" << KFUtility::Format( "var datatable = configreader.ReadFile(IConfig._path + \"%s.csv\");", filename.c_str() ) << "\n";
		csharpfile << "\t\t\t" << "if(datatable == null)" << "\n";
		csharpfile << "\t\t\t" << "{" << "\n";
		csharpfile << "\t\t\t\t" << "return false;" << "\n";
		csharpfile << "\t\t\t" << "}" << "\n";
		csharpfile << "\n";

		csharpfile << "\t\t\t" << "foreach(var row in datatable.Rows)" << "\n";
		csharpfile << "\t\t\t" << "{" << "\n";
		csharpfile << "\t\t\t\t" << KFUtility::Format( "var setting = new %sSetting();", kffile->_class._class_name.c_str() ) << "\n";

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
				csharpfile << KFUtility::Format( "setting.%s = datareader.GetValue<%s>(row[\"%s\"].ToString());", attribute->_name.c_str(),
					maintype.c_str(), attribute->_name.c_str() );
			}
			else
			{
				csharpfile << KFUtility::Format( "setting.%s = datareader.Get%s%s(row[\"%s\"].ToString());", attribute->_name.c_str(),
					maintype.c_str(), subtype.c_str(), attribute->_name.c_str() );
			}		
			csharpfile << "\n";
		}

		csharpfile << "\t\t\t\t";
		csharpfile << KFUtility::Format( "_settings[setting.%s] = setting;", kffile->_class._attributes[ 1 ]._name.c_str() );
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
			if (iter >= 'A' && iter <= 'Z' )
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