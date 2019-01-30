#include "KFExcelSheet.h"

namespace KFrame
{
    KFExcelSheet::KFExcelSheet()
    {

    }

    KFExcelSheet::~KFExcelSheet()
    {
        for ( auto cell : _cells )
        {
            delete cell;
        }
        _cells.clear();
    }


    KFExcelCell* KFExcelSheet::FindCell( int32 row, int32 col )
    {
        if ( row < _dimension._first_row || row > _dimension._last_row )
        {
            return nullptr;
        }

        if ( col < _dimension._first_col || col > _dimension._last_col )
        {
            return nullptr;
        }

        return _cells[ ToIndex( row, col ) ];
    }


    int32 KFExcelSheet::ToIndex( int32 row, int32 col )
    {
        return ( row - 1 ) * ( _dimension._last_col - _dimension._first_col + 1 ) + ( col - _dimension._first_col );
    }


}