#include "KFrame.h"
#include "KFConstantData.hpp"

namespace KFrame
{
    void KFConstantData::AddConstant( const std::string& name, uint32 key, uint32 value )
    {
        auto constant_key = ConstantKey( name, key );
        auto constant = _constant_list.Create( constant_key );
        constant->_uint32_value = value;
    }

    void KFConstantData::AddConstant( const std::string& name, uint32 key, double value )
    {
        auto constant_key = ConstantKey( name, key );
        auto constant = _constant_list.Create( constant_key );
        constant->_double_value = value;
    }

    void KFConstantData::AddConstant( const std::string& name, uint32 key, const std::string& value )
    {
        auto constant_key = ConstantKey( name, key );
        auto constant = _constant_list.Create( constant_key );
        constant->_str_value = value;
        constant->_uint32_value = __TO_UINT32__( value );
        constant->_double_value = __TO_DOUBLE__( value );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 获得常量
    std::shared_ptr<const KFConstant> KFConstantData::FindConstant( const std::string& name, uint32 key )
    {
        auto constant_key = ConstantKey( name, key );
        auto constant = _constant_list.Find( constant_key );
        if ( constant != nullptr )
        {
            return constant;
        }

        static auto _constant = __MAKE_SHARED__( KFConstant );
        return _constant;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // uint32配置
    uint32 KFConstantData::GetUInt32( const std::string& name, uint32 key )
    {
        auto constant = FindConstant( name, key );
        return constant->_uint32_value;
    }

    // double配置
    double KFConstantData::GetDouble( const std::string& name, uint32 key )
    {
        auto constant = FindConstant( name, key );
        return constant->_double_value;
    }

    // string配置
    const std::string& KFConstantData::GetString( const std::string& name, uint32 key )
    {
        auto constant = FindConstant( name, key );
        return constant->_str_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}
