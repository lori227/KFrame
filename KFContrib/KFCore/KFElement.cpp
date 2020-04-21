#include "KFElement.h"
#include "KFEnum.h"
#include "KFData.h"
#include "KFJson.h"

namespace KFrame
{
    bool KFValue::IsNeedShow()
    {
        return _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::Mask_Show );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    KFIntValue::KFIntValue()
    {
        _type = KFDataDefine::Type_UInt32;
    }

    void KFIntValue::SetValue( std::string value )
    {
        _min_value = KFUtility::SplitValue<uint64>( value, __RANGE_STRING__ );
        _max_value = KFUtility::SplitValue<uint64>( value, __RANGE_STRING__ );
        if ( _min_value >= _max_value )
        {
            _max_value = _min_value;
            _use_value = _min_value;
        }
    }

    uint64 KFIntValue::CalcUseValue( const KFDataSetting* kfsetting, double multiple )
    {
        _data_setting = kfsetting;
        if ( _min_value >= _max_value )
        {
            _use_value = _min_value;
        }
        else
        {
            _use_value = KFGlobal::Instance()->RandRange( ( uint32 )_min_value, ( uint32 )_max_value, 1u );
        }

        if ( multiple != 1.0f && _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::Mask_Multiple ) )
        {
            _use_value = static_cast< uint64 >( std::round( _use_value * multiple ) );
            _use_value = __MAX__( 1u, _use_value );
        }

        return _use_value;
    }

    uint64 KFIntValue::CalcUseValue( const KFDataSetting* kfsetting, const std::string& dataname, double multiple )
    {
        if ( _data_setting == nullptr )
        {
            _data_setting = kfsetting->_class_setting->FindSetting( dataname );
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
        _type = KFDataDefine::Type_String;
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
        _type = KFDataDefine::Type_Object;
        _element = __KF_NEW__( KFElementObject );
    }

    KFObjValue::~KFObjValue()
    {
        __KF_DELETE__( KFElementObject, _element );
    }
    //////////////////////////////////////////////////////////////////////////////////////

    bool KFElement::IsNeedShow() const
    {
        return _data_setting != nullptr && _data_setting->HaveMask( KFDataDefine::Mask_Show );
    }
    //////////////////////////////////////////////////////////////////////////////////////

    KFElementValue::~KFElementValue()
    {
        __KF_DELETE__( KFValue, _value );
    }

    bool KFElementValue::IsValue() const
    {
        return true;
    }

    void KFElementValue::SetValue( std::string value )
    {
        if ( KFUtility::IsNumber( value ) )
        {
            _value = __KF_NEW__( KFIntValue );
        }
        else
        {
            _value = __KF_NEW__( KFStrValue );
        }

        _value->SetValue( value );
    }

    const std::string& KFElementValue::ToString() const
    {
        static std::string _result;
        _result.clear();

        switch ( _value->_type )
        {
        case KFDataDefine::Type_UInt32:
            _result = __FORMAT__( "{{\"{}\":\"{}\"}}", _data_name, _value->GetUseValue() );
            break;
        case KFDataDefine::Type_String:
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

    void KFElementObject::SetValue( const std::string& dataname, std::string value )
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

        kfvalue->SetValue( value );
        _values.Insert( dataname, kfvalue );
    }

    uint64 KFElementObject::CalcValue( const KFDataSetting* kfsetting, const std::string& dataname, double multiple )
    {
        auto kfvalue = _values.Find( dataname );
        if ( kfvalue == nullptr || !kfvalue->IsType( KFDataDefine::Type_UInt32 ) )
        {
            return _invalid_int;
        }

        return kfvalue->CalcUseValue( kfsetting, dataname, multiple );
    }

    uint64 KFElementObject::GetValue( const std::string& dataname ) const
    {
        auto kfvalue = _values.Find( dataname );
        if ( kfvalue == nullptr || !kfvalue->IsType( KFDataDefine::Type_UInt32 ) )
        {
            return _invalid_int;
        }

        return kfvalue->GetUseValue();
    }

    KFObjValue* KFElementObject::CreateObjectValue( const std::string& dataname )
    {
        auto kfobject = _values.Find( dataname );
        if ( kfobject == nullptr )
        {
            kfobject = __KF_NEW__( KFObjValue );
            _values.Insert( dataname, kfobject );
        }

        return static_cast< KFObjValue* >( kfobject );
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
            auto kfvalue = iter.second;
            switch ( kfvalue->_type )
            {
            case KFDataDefine::Type_UInt32:
                __JSON_SET_VALUE__( kfobject, iter.first, kfvalue->GetUseValue() );
                break;
            case KFDataDefine::Type_String:
                __JSON_SET_VALUE__( kfobject, iter.first, kfvalue->GetValue() );
                break;
            case KFDataDefine::Type_Object:
            {
                __JSON_OBJECT__( kfchild );
                auto objvalue = static_cast< KFObjValue* >( kfvalue );
                for ( auto& iter : objvalue->_element->_values._objects )
                {
                    auto kfvalue = iter.second;
                    switch ( kfvalue->_type )
                    {
                    case KFDataDefine::Type_UInt32:
                        __JSON_SET_VALUE__( kfchild, iter.first, kfvalue->GetUseValue() );
                        break;
                    case KFDataDefine::Type_String:
                        __JSON_SET_VALUE__( kfchild, iter.first, kfvalue->GetValue() );
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
        for ( auto kfelenemt : _element_list )
        {
            __KF_DELETE__( KFElement, kfelenemt );
        }
        _element_list.clear();
    }

    void KFElements::SetOperate( uint32 operate )
    {
        for ( auto kfelement : _element_list )
        {
            kfelement->SetOperate( operate );
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

        rapidjson::Document kfjson;
        kfjson.Parse( data.c_str() );
        if ( kfjson.HasParseError() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] parse error", data );
            return false;
        }

        if ( !kfjson.IsArray() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] is not array", data );
            return false;
        }

        _str_element = data;
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
                kfelement->SetValue( jsonvalue.GetString() );
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
                        __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] not string", data, childname );
                        continue;
                    }

                    std::string strname = childname;
                    if ( strname == __STRING__( id ) )
                    {
                        kfelement->_config_id = __TO_UINT32__( jsonchild.GetString() );
                    }
                    else
                    {
                        auto parentname = KFUtility::SplitString( strname, __RANGE_STRING__ );
                        auto dataname = KFUtility::SplitString( strname, __RANGE_STRING__ );
                        if ( dataname.empty() )
                        {
                            kfelement->SetValue( parentname, jsonchild.GetString() );
                        }
                        else
                        {
                            auto kfobjvalue = kfelement->CreateObjectValue( parentname );
                            kfobjvalue->_element->SetValue( dataname, jsonchild.GetString() );
                        }
                    }
                }

                _element_list.push_back( kfelement );
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
        for ( auto kfelement : _element_list )
        {
            if ( !kfelement->IsValue() )
            {
                continue;
            }

            auto kfelementvalue = static_cast< KFElementValue* >( kfelement );
            if ( kfelementvalue->_value->IsType( KFDataDefine::Type_UInt32 ) )
            {
                auto value = static_cast< uint64 >( kfelementvalue->_value->GetUseValue() * multiple );

                __JSON_OBJECT__( kfobject );
                __JSON_SET_VALUE__( kfobject, kfelement->_data_name, __TO_STRING__( value ) );
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
