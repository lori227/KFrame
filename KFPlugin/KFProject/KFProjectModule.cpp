#include "KFProjectModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    enum KFRewardEnum
    {
        Element = 1,	// 默认的字串
        CodeJson = 2,	// 简化的json字串[1, 2, 3]
    };

    void KFProjectModule::InitModule()
    {
        KFGlobal::Instance()->RegisterParseToElementStringFunction( this, &KFProjectModule::ParseString );
        KFGlobal::Instance()->RegisterIntStringFunction( this, &KFProjectModule::FormatIntString );
        KFGlobal::Instance()->RegisterStrStringFunction( this, &KFProjectModule::FormatStrString );
        KFGlobal::Instance()->RegisterDataToElementStringFunction( this, &KFProjectModule::FormatDataString );
    }

    void KFProjectModule::BeforeRun()
    {
        auto global = KFGlobal::Instance();

    }

    void KFProjectModule::AfterLoad()
    {
        auto global = KFGlobal::Instance();
        if ( KFConstantConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFConstantConfig::Instance()->_setting_list._objects )
            {
                auto setting = iter.second;
                global->AddConstant( iter.first, 0u, setting->_value );
            }
        }

        if ( KFProjectConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFProjectConfig::Instance()->_setting_list._objects )
            {
                auto setting = iter.second;
                global->AddConstant( setting->_name, 0, setting->_value );
            }

            global->_project_time = global->GetUInt32( __STRING__( projecttime ) );
            global->_array_index = global->GetUInt32( __STRING__( arrayindex ) );
        }

        if ( KFUuidConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFUuidConfig::Instance()->_setting_list._objects )
            {
                auto setting = iter.second;
                global->AddUuidData( setting->_id, setting->_time, setting->_zone, setting->_worker, setting->_seq );
            }
        }

        if ( KFTemplateConfig::Instance()->_load_ok )
        {
            _id_name_list.clear();
            for ( auto& iter : KFTemplateConfig::Instance()->_setting_list._objects )
            {
                auto setting = iter.second;

                auto id = CalcTypeCodeValue( setting->_type, setting->_code );
                _id_name_list[ id ] = setting->_id;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFProjectModule::CalcTypeCodeValue( uint64 type, uint64 code )
    {
#define __TYPE_VALUE__ 10000000000

        uint64 result = 0u;
        switch ( type )
        {
        case KFMsg::CurrenyType:
            result = type * __TYPE_VALUE__ + code;
            break;
        default:
            result = type * __TYPE_VALUE__;
            break;
        }

        return result;
    }
    const std::string& KFProjectModule::FormatIntString( const std::string& data_name, uint32 data_value, uint32 data_id )
    {
        return FormatStrString( data_name, __TO_STRING__( data_value ), data_id );
    }

    const std::string& KFProjectModule::FormatStrString( const std::string& data_name, const std::string& data_value, uint32 data_id )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        auto setting = KFTemplateConfig::Instance()->FindSetting( data_name );
        if ( setting != nullptr )
        {
            _str_element = "[";
            _str_element += __FORMAT__( setting->_template, data_value, data_id );
            _str_element += "]";
        }
        else
        {
            __LOG_ERROR__( "data_name=[{}] no template", data_name );
        }

        return _str_element;
    }

    const std::string& KFProjectModule::FormatDataString( const KFElementData& element_data )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        auto index = 0u;
        _str_element = "[";
        for ( auto& tuple_data : element_data._data_list )
        {
            auto& data_name = std::get<0>( tuple_data );
            auto& data_value = std::get<1>( tuple_data );
            auto& data_id = std::get<2>( tuple_data );

            auto template_setting = KFTemplateConfig::Instance()->FindSetting( data_name );
            if ( template_setting == nullptr )
            {
                __LOG_ERROR__( "data_name=[{}] no template", data_name );
                continue;
            }

            if ( index > 0u )
            {
                _str_element += ",";
            }

            ++index;
            _str_element += __FORMAT__( template_setting->_template, data_value, data_id );
        }

        _str_element += "]";
        return _str_element;
    }

    const std::string& KFProjectModule::ParseString( const std::string& data )
    {
        static auto reward_type = KFGlobal::Instance()->GetUInt32( __STRING__( rewardtype ) );
        switch ( reward_type )
        {
        case KFRewardEnum::CodeJson:
            return ParseCodeString( data );
            break;
        }

        return data;
    }

    // [1,1,2]
    const std::string& KFProjectModule::ParseCodeString( const std::string& data )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        // 将字符串解析成数组
        __JSON_PARSE_STRING__( json_data, data );
        if ( !json_data.IsArray() )
        {
            return _str_element;
        }

        _str_element = "[";
        auto size = __JSON_ARRAY_SIZE__( json_data );
        for ( uint32 i = 0u; i < size; ++i )
        {
            auto& array_data = __JSON_ARRAY_INDEX__( json_data, i );
            if ( !array_data.IsArray() )
            {
                continue;
            }

            auto len = __JSON_ARRAY_SIZE__( array_data );
            if ( len != 3u )
            {
                continue;
            }

            auto type = array_data[ 0 ].GetUint();
            auto code = array_data[ 1 ].GetUint();
            auto num = array_data[ 2 ].GetUint();

            auto& reward = FormatRewardString( type, code, num );
            if ( reward.empty() )
            {
                continue;
            }

            if ( i > 0u )
            {
                _str_element += ",";
            }

            _str_element += reward;
        }
        _str_element += "]";

        return _str_element;
    }


    const std::string& KFProjectModule::FormatRewardString( uint32 type, uint32 code, uint32 num )
    {
        auto id_value = CalcTypeCodeValue( type, code );
        auto iter = _id_name_list.find( id_value );
        if ( iter == _id_name_list.end() )
        {
            return _invalid_string;
        }

        auto setting = KFTemplateConfig::Instance()->FindSetting( iter->second );
        if ( setting == nullptr )
        {
            return _invalid_string;
        }

        static std::string _str_element;
        _str_element = __FORMAT__( setting->_template, num, code );
        return _str_element;
    }
}