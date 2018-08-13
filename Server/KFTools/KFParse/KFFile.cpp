#include "KFFile.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFFile::AddData( KFData& data )
    {
        _datas.push_back( data );
        return true;
    }

    bool KFFile::AddData( uint32 index, const std::string& value )
    {
        KFData data;
        data.AddData( index, value );
        return AddData( data );
    }
}