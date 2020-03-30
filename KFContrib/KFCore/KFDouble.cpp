#include "KFDouble.h"

namespace KFrame
{
    KFDouble::KFDouble()
    {
        _data_type = KFDataDefine::Type_Double;
    }

    double KFDouble::GetDouble()
    {
        return _data;
    }

    double KFDouble::SetDouble( double value )
    {
        _data = value;
        return _data;
    }

    void KFDouble::Reset( bool isdelete /* = true */ )
    {
        _data = 0.0f;
        KFData::Reset( isdelete );
    }

    bool KFDouble::IsValid()
    {
        return _data != 0.0f;
    }

    void KFDouble::InitData()
    {
        if ( _data_setting->_int_init_value != 0.0f )
        {
            _data = _data_setting->_int_init_value;
        }
    }

    void KFDouble::CopyFrom( KFData* kfother )
    {
        Set( kfother->Get<double>() );
    }

    void KFDouble::SaveTo( KFData* kfother )
    {
        kfother->Set( Get<double>() );
    }

    std::string KFDouble::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFDouble::FromString( const std::string& value )
    {
        _data = __TO_DOUBLE__( value );
    }
}