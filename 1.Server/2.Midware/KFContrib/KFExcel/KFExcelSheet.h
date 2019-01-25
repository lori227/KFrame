#ifndef __KF_EXCEL_SHEET_H__
#define __KF_EXCEL_SHEET_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFExcelCell
    {
    public:
        std::string _value;
        std::string _type;
    };

    class KFExcelRange
    {
    public:
        int32 _first_row;
        int32 _last_row;
        int32 _first_col;
        int32 _last_col;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class KFExcelSheet
    {
    public:
        KFExcelSheet();
        ~KFExcelSheet();

        KFExcelCell* FindCell( int32 row, int32 col );
        int32 ToIndex( int32 row, int32 col );


    public:
        int32 _sheet_id;
        bool _visible;
        std::string _rid;
        std::string _path;
        std::string _name;

        KFExcelRange _dimension;
        std::vector< KFExcelCell* > _cells;
    };

    /////////////////////////////////////////////////////////////////////////////////////

}

#endif
