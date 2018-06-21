#include "KFConvert.h"
#include "iconv.h"

namespace KFrame
{
	static const char* _gb_code = "gb2312";
	static const char* _utf8_code = "utf-8";

	std::string KFConvert::ToCovert( void* iconvhandle, const std::string& source )
	{
		auto insize = source.size();
		const char* inbuff = source.data();

		static const uint32 _length = 1024 * 10;
		
		char iconvbuff[ _length ];
		auto outbuff = iconvbuff;
		uint32 iconvsize = _length;

		auto iconvresult = iconv( iconvhandle, (char**)(&inbuff), (size_t*)&insize, (char**)(&outbuff), (size_t*)&iconvsize );

		//关闭字符集转换
		iconv_close( iconvhandle );

		std::string result;
		result.assign( iconvbuff, _length - iconvsize );
		return result;
	}

	std::string KFConvert::ToUTF8( const std::string& source )
	{
		//打开字符集转换
		auto iconvhandle = iconv_open( _utf8_code, _gb_code );
		if ( iconvhandle == (iconv_t)-1 )
		{
			return "";
		}

		return ToCovert( iconvhandle, source );
	}

	std::string KFConvert::ToAscii( const std::string& source )
	{
		//打开字符集转换
		auto iconvhandle = iconv_open( _gb_code, _utf8_code );
		if ( iconvhandle == (iconv_t)-1 )
		{
			return "";
		}

		return ToCovert( iconvhandle, source );
	}
}