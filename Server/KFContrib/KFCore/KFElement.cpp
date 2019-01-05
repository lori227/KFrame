#include "KFElement.h"
#include "KFEnum.h"
#include "KFData.h"
#include "KFJson.h"

namespace KFrame
{
    bool KFValue::SetValue( std::string value )
    {
        _min_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        _max_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        if ( _min_value >= _max_value )
        {
            _max_value = _min_value;
            _use_value = _min_value;
        }

        return _min_value != _max_value;
    }

    uint64 KFValue::CalcValue( float multiple )
    {
        if ( _max_value > _min_value  )
        {
            _use_value = KFGlobal::Instance()->RandInRange( static_cast< uint32 >( _min_value ), static_cast< uint32 >( _max_value ), 1 );
        }

        _use_value = static_cast< uint64 >( _use_value * multiple );
        return _use_value;
    }

    uint64 KFValue::GetValue()
    {
        return _use_value;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    uint64 KFElementVale::CalcValue( float multiple /* = 1.0f */ )
    {
        if ( multiple != 1.0f )
        {
            _parent->_is_change_value = true;
        }

        return _value.CalcValue( multiple );
    }

    uint64 KFElementObject::CalcValue( const std::string& name )
    {
        auto kfvalue = _values.Find( name );
        if ( kfvalue == nullptr )
        {
            return 0u;
        }

        return kfvalue->CalcValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////
    KFElements::KFElements()
    {
        _is_change_value = false;
        _is_rand_value = false;
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

    //[{ "money":"1111"}, {"diamon":"2222-3333"}, {"item":{"id":1,"count":2 }} ]
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
                auto kfelement = __KF_NEW__( KFElementVale );
                kfelement->_data_name = name;
                _is_rand_value |= kfelement->_value.SetValue( jsonvalue.GetString() );
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
                        auto kfvalue = __KF_NEW__( KFValue );
                        _is_rand_value |= kfvalue->SetValue( jsonchild.GetString() );
                        kfelement->_values.Insert( childname, kfvalue );
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

    const std::string& KFElements::Serialize() const
    {
        //if ( !_is_rand_agent && !_is_add_value && multiple == 1.0f )
        // {
        //   return _string;
        //  }

        //// 重新格式化
        static std::string _show_string;
        //_show_string.clear();

        //KFJson kfjson;
        //for ( auto kfagent : _agents )
        //{
        //    KFJson kfdata;
        //    for ( auto& iter : kfagent->_datas._objects )
        //    {
        //        auto kfvalue = iter.second;
        //        kfdata[ iter.first ] = __TO_STRING__( kfvalue->GetUseValue() );
        //    }

        //    if ( kfagent->_config_id != _invalid_int )
        //    {
        //        kfdata[ __KF_STRING__( id ) ] = __TO_STRING__( kfagent->_config_id );
        //    }

        //    KFJson kfreward;
        //    kfreward[ kfagent->_data_name ] = kfdata;
        //    kfjson.append( kfreward );
        //}

        //_show_string = kfjson.Serialize();
        //KFUtility::ReplaceString( _show_string, ",", "|" );

        return _show_string;
    }
}
