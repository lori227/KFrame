#include "KFElement.h"
#include "KFEnum.h"
#include "KFData.h"
#include "KFJson.h"

namespace KFrame
{
    bool KFIntValue::SetValue( std::string value )
    {
        _min_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        _max_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        if ( _min_value >= _max_value )
        {
            _max_value = _min_value;
            _str_value = __TO_STRING__( _min_value );
            _use_value = _str_value;
        }

        return _min_value != _max_value;
    }

    const std::string& KFIntValue::CalcValue( float multiple )
    {
        if ( _min_value >= _max_value )
        {
            // 没有随机
            if ( multiple == 1.0f )
            {
                // 没有倍数
                return _str_value;
            }

            _use_value = __TO_STRING__( static_cast< uint64 >( _min_value * multiple ) );
            return _use_value;
        }

        // 随机
        auto usevalue =  KFGlobal::Instance()->RandRange( static_cast< uint32 >( _min_value ), static_cast< uint32 >( _max_value ), 1 );
        _use_value = __TO_STRING__( static_cast< uint64 >( usevalue * multiple ) );
        return _use_value;
    }

    uint64 KFIntValue::CalcUInt64( float multiple )
    {
        if ( _min_value >= _max_value )
        {
            // 没有随机
            if ( multiple == 1.0f )
            {
                // 没有倍数
                return _min_value;
            }

            auto usevalue = static_cast< uint64 >( _min_value * multiple );
            _use_value = __TO_STRING__( usevalue );
            return usevalue;
        }

        auto usevalue = KFGlobal::Instance()->RandRange( static_cast< uint32 >( _min_value ), static_cast< uint32 >( _max_value ), 1 );
        _use_value = __TO_STRING__( static_cast< uint64 >( usevalue * multiple ) );
        return usevalue;
    }

    const std::string& KFIntValue::GetValue( float multiple )
    {
        if ( _min_value >= _max_value && multiple == 1.0f )
        {
            return _str_value;
        }

        return _use_value;
    }

    bool KFStrValue::SetValue( std::string value )
    {
        _str_value = value;
        _use_value = value;
        return false;
    }

    const std::string& KFStrValue::CalcValue( float multiple )
    {
        return _str_value;
    }

    uint64 KFStrValue::CalcUInt64( float multiple )
    {
        return _invalid_int;
    }

    const std::string& KFStrValue::GetValue( float multiple )
    {
        return _str_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    KFElementValue::KFElementValue()
    {
        _type = KFElementEnum::Value;
        _value = nullptr;
    }

    KFElementValue::~KFElementValue()
    {
        __KF_DELETE__( KFValue, _value );
    }

    bool KFElementValue::SetValue( std::string value )
    {
        if ( KFUtility::IsNumber( value ) )
        {
            _value = __KF_NEW__( KFIntValue );
        }
        else
        {
            _value = __KF_NEW__( KFStrValue );
        }

        return _value->SetValue( value );
    }

    const std::string& KFElementValue::CalcValue( float multiple )
    {
        return _value->CalcValue( multiple );
    }

    uint64 KFElementValue::CalcUInt64( float multiple )
    {
        return _value->CalcUInt64( multiple );
    }

    const std::string& KFElementValue::GetValue( float multiple )
    {
        return _value->GetValue( multiple );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    bool KFElementObject::SetValue( const std::string& dataname, std::string value )
    {
        KFValue* kfvalue = nullptr;
        if ( KFUtility::IsNumber( value ) )
        {
            kfvalue = __KF_NEW__( KFIntValue );
        }
        else
        {
            kfvalue = __KF_NEW__( KFStrValue );
        }

        _values.Insert( dataname, kfvalue );
        return kfvalue->SetValue( value );
    }

    const std::string& KFElementObject::CalcValue( const std::string& name, float multiple )
    {
        auto kfvalue = _values.Find( name );
        if ( kfvalue == nullptr )
        {
            return _invalid_str;
        }

        return kfvalue->CalcValue( multiple );
    }

    uint64 KFElementObject::CalcUInt64( const std::string& name, float multiple )
    {
        auto kfvalue = _values.Find( name );
        if ( kfvalue == nullptr )
        {
            return _invalid_int;
        }

        return kfvalue->CalcUInt64( multiple );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    KFElements::KFElements()
    {

    }

    KFElements::~KFElements()
    {
        Cleanup();
    }

    void KFElements::Cleanup()
    {
        for ( auto kfelenemt : _element_list )
        {
            __KF_DELETE__( KFElement, kfelenemt );
        }
        _element_list.clear();
    }

    void KFElements::AddElement( KFElement* kfelement )
    {
        kfelement->_parent = this;
        _element_list.push_back( kfelement );
    }

    //[{"money":"1111"}, {"diamon":"2222-3333"}, {"item":{"id":1,"count":2 }} ]
    bool KFElements::Parse( const std::string& data, const char* function, uint32 line )
    {
        if ( data.empty() )
        {
            return false;
        }

        Cleanup();
        _data = data;

        __JSON_PARSE_STRING__( kfjson, data );
        if ( kfjson.HasParseError() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] parse error!", data );
            return false;
        }

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( auto i = 0u; i < size; ++i )
        {
            auto& jsonobject = __JSON_ARRAY_INDEX__( kfjson, i );
            if ( !jsonobject.IsObject() )
            {
                continue;
            }

            auto iter = jsonobject.MemberBegin();
            auto name = iter->name.GetString();
            auto& jsonvalue = iter->value;
            if ( jsonvalue.IsString() )
            {
                auto kfelement = __KF_NEW__( KFElementValue );
                kfelement->_data_name = name;
                _is_rand_value |= kfelement->SetValue( jsonvalue.GetString() );
                _element_list.push_back( kfelement );
            }
            else if ( jsonvalue.IsObject() )
            {
                auto kfelement = __KF_NEW__( KFElementObject );
                kfelement->_data_name = name;
                for ( auto iter = jsonvalue.MemberBegin(); iter != jsonvalue.MemberEnd(); ++iter )
                {
                    auto childname = iter->name.GetString();
                    auto& jsonchild = iter->value;
                    if ( !jsonchild.IsString() )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] not string!", data, childname );
                        continue;
                    }

                    if ( childname == __KF_STRING__( id ) )
                    {
                        kfelement->_config_id = KFUtility::ToValue< uint32 >( jsonchild.GetString() );
                    }
                    else
                    {
                        _is_rand_value |= kfelement->SetValue( childname, jsonchild.GetString() );
                    }
                }

                _element_list.push_back( kfelement );
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] error!", data, name );
            }
        }

        return true;
    }

    void KFElements::SetOperate( uint32 operate )
    {
        for ( auto kfelement : _element_list )
        {
            kfelement->SetOperate( operate );
        }
    }

    const std::string& KFElements::Serialize( float multiple ) const
    {
        if ( !_is_rand_value && multiple == 1.0f )
        {
            return _data;
        }

        // 重新格式化
        static std::string _show_string;
        _show_string.clear();

        __JSON_DOCUMENT__( kfjson );
        kfjson.SetArray();

        for ( auto kfelement : _element_list )
        {
            if ( kfelement->IsValue() )
            {
                auto kfelementvalue = static_cast< KFElementValue* >( kfelement );
                auto& usevalue = kfelementvalue->GetValue( multiple );

                __JSON_OBJECT__( kfdata );
                __JSON_SET_VALUE__( kfdata, kfelementvalue->_data_name, usevalue );
                __JSON_ADD_VALUE__( kfjson, kfdata );
            }
            else if ( kfelement->IsObject() )
            {
                auto kfelementobject = static_cast< KFElementObject* >( kfelement );

                __JSON_OBJECT__( kfchild );
                if ( kfelementobject->_config_id != 0u )
                {
                    auto strid = __TO_STRING__( kfelementobject->_config_id );
                    __JSON_SET_VALUE__( kfchild, __KF_STRING__( id ), strid );
                }

                for ( auto& iter : kfelementobject->_values._objects )
                {
                    auto& usevalue = iter.second->GetValue( multiple );
                    __JSON_SET_VALUE__( kfchild, iter.first, usevalue );
                }

                __JSON_OBJECT__( kfdata );
                __JSON_SET_VALUE__( kfdata, kfelementobject->_data_name, kfchild );
                __JSON_ADD_VALUE__( kfjson, kfdata );
            }
        }

        _show_string = __JSON_SERIALIZE__( kfjson );
        return _show_string;
    }
}
