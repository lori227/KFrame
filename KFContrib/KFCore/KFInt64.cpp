#include "KFInt64.hpp"

namespace KFrame
{
    KFInt64::KFInt64()
    {
        _data_type = KFDataDefine::Type_Int64;
    }

    int64 KFInt64::GetInt64()
    {
        return _data;
    }

    int64 KFInt64::SetInt64( int64 value )
    {
        _data = value;
        if ( _data > ( int64 )_data_setting->_int_max_value )
        {
            _data = ( int64 )_data_setting->_int_max_value;
        }
        else if ( _data < ( int64 )_data_setting->_int_min_value )
        {
            _data = ( int64 )_data_setting->_int_min_value;
        }

        return _data;
    }

    void KFInt64::Reset( bool isdelete /* = true */ )
    {
        _data = _invalid_int;
        KFData::Reset( isdelete );
    }

    bool KFInt64::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFInt64::IsFull()
    {
        return _data >= ( int64 )_data_setting->_int_max_value;
    }

    void KFInt64::InitData()
    {
        _data = _data_setting->_int_init_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFInt64::CopyFrom( KFData* kfother )
    {
        Set( kfother->Get<int64>() );
    }

    void KFInt64::SaveTo( KFData* kfother )
    {
        kfother->Set( Get<int64>() );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFInt64::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFInt64::FromString( const std::string& value )
    {
        SetInt64( __TO_INT64__( value ) );
    }
}
