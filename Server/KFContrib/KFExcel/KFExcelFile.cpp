#include "KFExcelFile.h"
#include "KFExcelZip.hpp"

namespace KFrame
{
	KFExcelFile::KFExcelFile()
	{
		_zip = nullptr;
	}

	KFExcelFile::~KFExcelFile()
	{
		if ( _zip != nullptr )
		{
			delete _zip;
			_zip = nullptr;
		}

		for ( auto sheet : _sheets )
		{
			delete sheet;
		}
		_sheets.clear();
	}

	void KFExcelFile::ReadWorkBook( const char* file )
	{
		rapidxml::xml_document<> doc;

		_zip->OpenXML( file, doc );

		rapidxml::xml_node<>* e = doc.first_node( "workbook" );
		e = e->first_node( "sheets" );
		e = e->first_node( "sheet" );

		while ( e )
		{
			KFExcelSheet* sheet = new KFExcelSheet();

			sheet->_name = e->first_attribute( "name" )->value();
			sheet->_rid = e->first_attribute( "r:id" )->value();
			sheet->_sheet_id = atoi(e->first_attribute( "sheetId" )->value());
			sheet->_visible = (e->first_attribute( "state" ) && !strcmp( e->first_attribute( "state" )->value(), "hidden" ));

			e = e->next_sibling( "sheet" );

			_sheets.push_back( sheet );
		}
	}

	void KFExcelFile::ReadWorkBookRels( const char* file )
	{
		rapidxml::xml_document<> doc;

		_zip->OpenXML( file, doc );
		rapidxml::xml_node<>* e = doc.first_node( "Relationships" );
		e = e->first_node( "Relationship" );

		while ( e )
		{
			const char* rid = e->first_attribute( "Id" )->value();

			for ( auto sheet : _sheets )
			{
				if ( sheet->_rid == rid )
				{
					sheet->_path = "xl/" + std::string( e->first_attribute( "Target" )->value() );
					break;
				}
			}

			e = e->next_sibling( "Relationship" );
		}
	}

	void KFExcelFile::ReadSharedStrings( const char* file )
	{
		rapidxml::xml_document<> doc;

		if ( !_zip->OpenXML( file, doc ) )
		{
			return;
		}

		rapidxml::xml_node<>* e = doc.first_node( "sst" );
		e = e->first_node( "si" );

		rapidxml::xml_node<> *t, *r;
		int i = 0;

		while ( e )
		{
			t = e->first_node( "t" );
			i++;
			if ( t )
			{
				const char* text = t->value();
				_shared_string.push_back( text ? text : "" );
			}
			else
			{
				r = e->first_node( "r" );
				std::string value;
				while ( r )
				{
					t = r->first_node( "t" );
					value += t->value();
					r = r->next_sibling( "r" );
				}
				_shared_string.push_back( value );
			}
			e = e->next_sibling( "si" );
		}
	}

	void KFExcelFile::ReadStyles( const char* file )
	{
		rapidxml::xml_document<> doc;

		_zip->OpenXML( file, doc );
	}

	void KFExcelFile::ParseCell( const std::string& value, int& row, int& col )
	{
		int index = 0;
		col = 0;

		int arr[ 10 ];

		while ( index < (int)value.length() )
		{
			if ( isdigit( value[ index ] ) ) break;
			arr[ index ] = value[ index ] - 'A' + 1;
			index++;
		}

		for ( int i = 0; i < index; i++ )
		{
			col += (int)(arr[ i ] * pow( 26, index - i - 1 ));
		}

		row = atoi( value.c_str() + index );
	}

	void KFExcelFile::ParseRange( const std::string& value, KFExcelRange& range )
	{
		size_t index = value.find_first_of( ':' );

		if ( index != std::string::npos )
		{
			ParseCell( value.substr( 0, index ), range._first_row, range._first_col );
			ParseCell( value.substr( index + 1 ), range._last_row, range._last_col );
		}
		else
		{
			ParseCell( value, range._first_row, range._first_col );
			range._last_col = range._first_col;
			range._last_row = range._first_row;
		}
	}

	void KFExcelFile::ReadSheet( KFExcelSheet* sheet )
	{
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<> *root, *row, *c, *v, *d;

		_zip->OpenXML( sheet->_path.c_str(), doc );

		root = doc.first_node( "worksheet" );

		d = root->first_node( "dimension" );
		if ( d != nullptr )
		{
			ParseRange( d->first_attribute( "ref" )->value(), sheet->_dimension );
		}

		row = root->first_node( "sheetData" );
		row = row->first_node( "row" );

		int vecsize = (sheet->_dimension._last_col - sheet->_dimension._first_col + 1) * (sheet->_dimension._last_row - sheet->_dimension._first_row + 1);

		sheet->_cells.resize( vecsize );


		while ( row )
		{
			int rowIdx = atoi(row->first_attribute( "r" )->value());
			c = row->first_node( "c" );

			while ( c )
			{
				int colIdx = 0;
				ParseCell( c->first_attribute( "r" )->value(), rowIdx, colIdx );
				int index = sheet->ToIndex( rowIdx, colIdx );

				const char *s, *t;

				v = c->first_node( "v" );

				KFExcelCell* cell = new KFExcelCell();

				if ( v )
				{
					s = v->value();
					if ( c->first_attribute( "t" ) )
					{
						t = c->first_attribute( "t" )->value();
						if ( !strcmp( t, "s" ) )
						{
							cell->_value = (char*)_shared_string[ atoi( s ) ].c_str();
							cell->_type = "string";
						}
						else if ( !strcmp( t, "b" ) )
						{
							if ( !strcmp( s, "0" ) )
							{
								cell->_value = "FALSE";
							}
							else
							{
								cell->_value = "TRUE";
							}
							cell->_type = "bool";
						}
					}
					else
					{
						cell->_type = "unknow";
						cell->_value = (char*)s;
					}
				}
				sheet->_cells[ index ] = cell;
				c = c->next_sibling( "c" );
			}

			row = row->next_sibling( "row" );
		}
	}



	bool KFExcelFile::Open( const char* file )
	{
		_zip = new KFExcelZip();

		if ( !_zip->Open( file ) )
		{
			return false;
		}

		ReadWorkBook( "xl/workbook.xml" );
		ReadWorkBookRels( "xl/_rels/workbook.xml.rels" );
		ReadSharedStrings( "xl/sharedStrings.xml" );
		ReadStyles( "styles.xml" );

		for ( auto sheet : _sheets )
		{
			ReadSheet( sheet );
		}

		return true;
	}


	KFExcelSheet* KFExcelFile::FindSheet( const char* name )
	{
		for ( auto sheet : _sheets )
		{
			if ( sheet->_name == name )
				return sheet;
		}

		return nullptr;
	}

	
}