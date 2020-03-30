#include "KFrame.h"
#include "KFConstantData.hpp"

namespace KFrame
{
    void KFConstantData::AddConstant( const std::string& name, uint32 key, uint32 value )
    {
        auto constkey = ConstantKey( name, key );
        auto kfconstant = _constant_list.Create( constkey );
        kfconstant->_uint32_value = value;
    }

    void KFConstantData::AddConstant( const std::string& name, uint32 key, double value )
    {
        auto constkey = ConstantKey( name, key );
        auto kfconstant = _constant_list.Create( constkey );
        kfconstant->_double_value = value;
    }

    void KFConstantData::AddConstant( const std::string& name, uint32 key, const std::string& value )
    {
        auto constkey = ConstantKey( name, key );
        auto kfconstant = _constant_list.Create( constkey );
        kfconstant->_str_value = value;
        kfconstant->_uint32_value = __TO_UINT32__( value );
        kfconstant->_double_value = __TO_DOUBLE__( value );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 获得常量
    const KFConstant* KFConstantData::FindConstant( const std::string& name, uint32 key )
    {
        auto constkey = ConstantKey( name, key );
        auto kfconstant = _constant_list.Find( constkey );
        if ( kfconstant == nullptr )
        {
            static KFConstant _constant_data;
            kfconstant = &_constant_data;
        }

        return kfconstant;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // uint32配置
    uint32 KFConstantData::GetUInt32( const std::string& name, uint32 key )
    {
        auto kfconstant = FindConstant( name, key );
        return kfconstant->_uint32_value;
    }

    // double配置
    double KFConstantData::GetDouble( const std::string& name, uint32 key )
    {
        auto kfconstant = FindConstant( name, key );
        return kfconstant->_double_value;
    }

    // string配置
    const std::string& KFConstantData::GetString( const std::string& name, uint32 key )
    {
        auto kfconstant = FindConstant( name, key );
        return kfconstant->_str_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}
