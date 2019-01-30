#ifndef __KF_EXCEL_ZIP_H__
#define __KF_EXCEL_ZIP_H__

#include "KFInclude.h"
#include "minizip/unzip.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_iterators.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"

namespace KFrame
{
	class ZipEntryInfo
	{
	public:
		unz_file_pos pos;
		uLong uncompressed_size;
	};

	class KFExcelZip
	{
	public:
		KFExcelZip();
		~KFExcelZip();

		bool Open( const char* file );
		bool OpenXML( const char* filename, rapidxml::xml_document<>& doc );

	private:
		char* GetFileData( const char* filename, unsigned long& size );

	public:
		std::map<std::string, ZipEntryInfo> _files;
		unzFile _zipFile;
	};
}


#endif
