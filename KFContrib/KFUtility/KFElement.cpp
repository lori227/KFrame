#include "KFDataSetting.h"

namespace KFrame
{
    bool KFValue::IsNeedShow()
    {
        return _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::DataMaskShow );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    KFIntValue::KFIntValue()
    {
        _type = KFDataDefine::DataTypeUInt32;
    }

    KFIntValue::~KFIntValue()
    {

    }

    void KFIntValue::SetValue( std::string value )
    {
        _range.SetValue( value );
    }

    uint64 KFIntValue::CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, double multiple )
    {
        _data_setting = setting;
        _use_value = _range.CalcValue();

        if ( !KFUtility::IsDoubleEqual( multiple, _default_multiple ) && _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::DataMaskMultiple ) )
        {
            _use_value = static_cast< uint64 >( std::round( _use_value * multiple ) );
            _use_value = __MAX__( 1u, _use_value );
        }

        return _use_value;
    }

    uint64 KFIntValue::CalcUseValue( std::shared_ptr<const KFDataSetting>& setting, const std::string& data_name, double multiple )
    {
        if ( _data_setting == nullptr )
        {
            _data_setting = setting->_class_setting->FindSetting( data_name );
        }

        return CalcUseValue( _data_setting, multiple );
    }

    uint64 KFIntValue::GetUseValue() const
    {
        return _use_value;
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    KFStrValue::KFStrValue()
    {
        _type = KFDataDefine::DataTypeString;
    }

    KFStrValue::~KFStrValue()
    {
    }

    void KFStrValue::SetValue( std::string value )
    {
        _str_value = value;
    }

    const std::string& KFStrValue::GetValue()
    {
        return _str_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    KFObjValue::KFObjValue()
    {
        _type = KFDataDefine::DataTypeObject;
        _element = __MAKE_SHARED__( KFElementObject );
    }

    KFObjValue::~KFObjValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////////////////
    bool KFElement::IsNeedShow() const
    {
        return _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::DataMaskShow );
    }
    //////////////////////////////////////////////////////////////////////////////////////
    KFElementValue::~KFElementValue()
    {
    }

    bool KFElementValue::IsValue() const
    {
        return true;
    }

    void KFElementValue::SetValue( std::string value )
    {
        if ( KFUtility::IsNumber( value ) )
        {
            _value = __MAKE_SHARED__( KFIntValue );
        }
        else
        {
            _value = __MAKE_SHARED__( KFStrValue );
        }

        _value->SetValue( value );
    }

    const std::string& KFElementValue::ToString() const
    {
        static std::string _result;
        _result.clear();

        switch ( _value->_type )
        {
        case KFDataDefine::DataTypeUInt32:
            _result = __FORMAT__( "{{\"{}\":\"{}\"}}", _data_name, _value->GetUseValue() );
            break;
        case KFDataDefine::DataTypeString:
            _result = __FORMAT__( "{{\"{}\":\"{}\"}}", _data_name, _value->GetValue() );
            break;
        default:
            break;
        }

        return _result;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    bool KFElementObject::IsObject() const
    {
        return true;
    }

    void KFElementObject::SetValue( const std::string& data_name, std::string value )
    {
        std::shared_ptr<KFValue> value_object = nullptr;
        if ( KFUtility::IsNumber( value ) )
        {
            value_object = __MAKE_SHARED__( KFIntValue );
        }
        else
        {
            value_object = __MAKE_SHARED__( KFStrValue );
        }

        value_object->SetValue( value );
        _values.Insert( data_name, value_object );
    }

    uint64 KFElementObject::CalcValue( std::shared_ptr<const KFDataSetting>& setting, const std::string& data_name, double multiple /* = _default_multiple */ )
    {
        auto value_object = _values.Find( data_name );
        if ( value_object == nullptr || !value_object->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            return _invalid_int;
        }

        return value_object->CalcUseValue( setting, data_name, multiple );
    }

    uint64 KFElementObject::GetValue( const std::string& data_name ) const
    {
        auto value_object = _values.Find( data_name );
        if ( value_object == nullptr || !value_object->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            return _invalid_int;
        }

        return value_object->GetUseValue();
    }

    std::shared_ptr<KFObjValue> KFElementObject::CreateObjectValue( const std::string& data_name )
    {
        auto object = _values.Find( data_name );
        if ( object == nullptr )
        {
            object = __MAKE_SHARED__( KFObjValue );
            _values.Insert( data_name, object );
        }

        return std::static_pointer_cast< KFObjValue >( object );
    }

    const std::string& KFElementObject::ToString() const
    {
        static std::string _result;
        _result.clear();

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_OBJECT__( kfobject );
        if ( _config_id != _invalid_int )
        {
            __JSON_SET_VALUE__( kfobject, __STRING__( id ), _config_id );
        }

        for ( auto& iter : _values._objects )
        {
            auto value_object = iter.second;
            switch ( value_object->_type )
            {
            case KFDataDefine::DataTypeUInt32:
                __JSON_SET_VALUE__( kfobject, iter.first, value_object->GetUseValue() );
                break;
            case KFDataDefine::DataTypeString:
                __JSON_SET_VALUE__( kfobject, iter.first, value_object->GetValue() );
                break;
            case KFDataDefine::DataTypeObject:
            {
                __JSON_OBJECT__( kfchild );
                auto obj_value = std::static_pointer_cast< KFObjValue >( value_object );
                for ( auto& iter : obj_value->_element->_values._objects )
                {
                    auto value = iter.second;
                    switch ( value->_type )
                    {
                    case KFDataDefine::DataTypeUInt32:
                        __JSON_SET_VALUE__( kfchild, iter.first, value->GetUseValue() );
                        break;
                    case KFDataDefine::DataTypeString:
                        __JSON_SET_VALUE__( kfchild, iter.first, value->GetValue() );
                        break;
                    }
                }
                __JSON_SET_VALUE__( kfobject, iter.first, kfchild );
            }
            }
        }

        __JSON_SET_VALUE__( kfjson, _data_name, kfobject );
        _result = __JSON_SERIALIZE__( kfjson );
        return _result;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    KFElements::~KFElements()
    {
        Clear();
    }

    bool KFElements::IsEmpty() const
    {
        return _element_list.empty();
    }

    void KFElements::Clear()
    {
        _element_list.clear();
    }

    void KFElements::SetOperate( uint32 operate )
    {
        for ( auto& element : _element_list )
        {
            element->SetOperate( operate );
        }
    }

    //[{"money":"1111"},{"diamon":"2222"},{"item":{"id":"1","count":"2"}}]
    bool KFElements::Parse( const std::string& data, const char* function, uint32 line )
    {
        Clear();
        if ( data.empty() )
        {
            return true;
        }

        rapidjson::Document json;
        json.Parse( data.c_str() );
        if ( json.HasParseError() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] parse error", data );
            return false;
        }

        if ( !json.IsArray() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] is not array", data );
            return false;
        }

        _str_element = data;
        auto size = __JSON_ARRAY_SIZE__( json );
        for ( auto i = 0u; i < size; ++i )
        {
            auto& json_object = __JSON_ARRAY_INDEX__( json, i );
            if ( !json_object.IsObject() )
            {
                continue;
            }

            auto iter = json_object.MemberBegin();
            auto name = iter->name.GetString();
            auto& json_value = iter->value;
            if ( json_value.IsString() )
            {
                auto element = __MAKE_SHARED__( KFElementValue );
                element->_data_name = name;
                element->SetValue( json_value.GetString() );
                _element_list.push_back( element );
            }
            else if ( json_value.IsObject() )
            {
                auto element = __MAKE_SHARED__( KFElementObject );
                element->_data_name = name;

                for ( auto iter = json_value.MemberBegin(); iter != json_value.MemberEnd(); ++iter )
                {
                    auto child_name = iter->name.GetString();
                    auto& json_child = iter->value;
                    if ( !json_child.IsString() )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] not string", data, child_name );
                        continue;
                    }

                    std::string name = child_name;
                    if ( name == __STRING__( id ) )
                    {
                        element->_config_id = __TO_UINT32__( json_child.GetString() );
                    }
                    else
                    {
                        auto parent_name = KFUtility::SplitString( name, __RANGE_STRING__ );
                        auto data_name = KFUtility::SplitString( name, __RANGE_STRING__ );
                        if ( data_name.empty() )
                        {
                            element->SetValue( parent_name, json_child.GetString() );
                        }
                        else
                        {
                            auto obj_value = element->CreateObjectValue( parent_name );
                            obj_value->_element->SetValue( data_name, json_child.GetString() );
                        }
                    }
                }

                _element_list.push_back( element );
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] error", data, name );
            }
        }

        return true;
    }

    const std::string& KFElements::GetElement() const
    {
        return _str_element;
    }

    const std::string& KFElements::CalcElement( double multiple ) const
    {
        if ( multiple == _default_multiple )
        {
            return _str_element;
        }

        static std::string _result;
        _result.clear();

        // 可以计算的都是比较简单的资源
        __JSON_ARRAY_DOCUMENT__( kfjson );
        for ( auto& element : _element_list )
        {
            if ( !element->IsValue() )
            {
                continue;
            }

            auto element_value = std::static_pointer_cast< KFElementValue >( element );
            if ( element_value->_value->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                auto value = static_cast< uint64 >( element_value->_value->GetUseValue() * multiple );

                __JSON_OBJECT__( kfobject );
                __JSON_SET_VALUE__( kfobject, element->_data_name, __TO_STRING__( value ) );
                __JSON_ADD_VALUE__( kfjson, kfobject );
            }
        }

        if ( __JSON_ARRAY_SIZE__( kfjson ) > 0u )
        {
            _result = __JSON_SERIALIZE__( kfjson );
        }

        return _result;
    }
}
