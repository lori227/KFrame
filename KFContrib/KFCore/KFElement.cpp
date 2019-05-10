#include "KFElement.h"
#include "KFEnum.h"
#include "KFData.h"
#include "KFJson.h"

namespace KFrame
{
    bool KFValue::IsNeedShow()
    {
        return _data_setting != nullptr && _data_setting->HaveFlagMask( KFDataDefine::Mask_Show );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFIntValue::IsInt()
    {
        return true;
    }

    void KFIntValue::SetValue( std::string value )
    {
        _min_value = KFUtility::SplitValue<uint32>( value, FUNCTION_RANGE_STRING );
        _max_value = KFUtility::SplitValue<uint32>( value, FUNCTION_RANGE_STRING );
        if ( _min_value >= _max_value )
        {
            _max_value = _min_value;
            _use_value = _min_value;
        }
    }

    uint32 KFIntValue::CalcUseValue( const KFDataSetting* kfsetting, float multiple )
    {
        _data_setting = kfsetting;
        if ( _min_value >= _max_value )
        {
            _use_value = _min_value;
        }
        else
        {
            _use_value = KFGlobal::Instance()->RandRange( _min_value, _max_value, 1 );
        }

        if ( multiple != 1.0f && _data_setting != nullptr && _data_setting->HaveFlagMask( KFDataDefine::Mask_Multiple ) )
        {
            _use_value = static_cast<uint32>( _use_value * multiple );
        }

        return _use_value;
    }

    uint32 KFIntValue::CalcUseValue( const KFClassSetting* kfsetting, const std::string& dataname, float multiple )
    {
        if ( _data_setting == nullptr )
        {
            _data_setting = kfsetting->FindDataSetting( dataname );
        }

        return CalcUseValue( _data_setting, multiple );
    }

    uint32 KFIntValue::GetUseValue()
    {
        return _use_value;
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    bool KFStrValue::IsString()
    {
        return true;
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
    //////////////////////////////////////////////////////////////////////////////////////

    bool KFElement::IsNeedShow() const
    {
        return _data_setting != nullptr && _data_setting->HaveFlagMask( KFDataDefine::Mask_Show );
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

        if ( _value->IsInt() )
        {
            _result = __FORMAT__( "{{\"{}\":\"{}\"}}", _data_name, _value->GetUseValue() );
        }
        else if ( _value->IsString() )
        {
            _result = __FORMAT__( "{{\"{}\":\"{}\"}}", _data_name, _value->GetValue() );
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

    uint32 KFElementObject::GetValue( const KFClassSetting* kfsetting, const std::string& dataname, float multiple )
    {
        auto kfvalue = _values.Find( dataname );
        if ( kfvalue == nullptr || kfvalue->IsString() )
        {
            return _invalid_int;
        }

        return kfvalue->CalcUseValue( kfsetting, dataname, multiple );
    }

    const std::string& KFElementObject::ToString() const
    {
        static std::string _result;
        _result.clear();

        __JSON_DOCUMENT__( kfjson );
        __JSON_OBJECT__( kfobject );
        if ( _config_id != _invalid_int )
        {
            __JSON_SET_VALUE__( kfobject, __KF_STRING__( id ), __TO_STRING__( _config_id ) );
        }

        for ( auto& iter : _values._objects )
        {
            auto kfvalue = iter.second;
            if ( kfvalue->IsInt() )
            {
                __JSON_SET_VALUE__( kfjson, iter.first, __TO_STRING__( kfvalue->GetUseValue() ) );
            }
            else if ( kfvalue->IsString() )
            {
                __JSON_SET_VALUE__( kfjson, iter.first, kfvalue->GetValue() );
            }
        }

        __JSON_SET_VALUE__( kfjson, _data_name, kfjson );
        _result = __JSON_SERIALIZE__( kfjson );
        return _result;
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

    void KFElements::SetOperate( uint32 operate )
    {
        for ( auto kfelement : _element_list )
        {
            kfelement->SetOperate( operate );
        }
    }

    //[{"money":"1111"}, {"diamon":"2222-3333"}, {"item":{"id":"1","count":"2"}} ]
    bool KFElements::Parse( const std::string& data, const char* function, uint32 line )
    {
        if ( data.empty() )
        {
            return false;
        }

        Cleanup();

        rapidjson::Document kfjson;
        kfjson.Parse( data.c_str() );
        if ( kfjson.HasParseError() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "data=[{}] parse error!", data );
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
                        __LOG_ERROR_FUNCTION__( function, line, "data=[{}] child[{}] not string!", data, childname );
                        continue;
                    }

                    if ( childname == __KF_STRING__( id ) )
                    {
                        kfelement->_config_id = KFUtility::ToValue< uint32 >( jsonchild.GetString() );
                    }
                    else
                    {
                        kfelement->SetValue( childname, jsonchild.GetString() );
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
}
