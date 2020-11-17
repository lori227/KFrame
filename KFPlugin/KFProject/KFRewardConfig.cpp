#include "KFRewardConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRewardConfig::ClearSetting()
    {
        _id_name_list.clear();
        KFConfigT< KFRewardSetting >::ClearSetting();
    }

    // 读取配置
    void KFRewardConfig::ReadSetting( KFXmlNode& xmlnode, KFRewardSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.ReadUInt32( "type" );
        kfsetting->_code = xmlnode.ReadUInt32( "code" );
        kfsetting->_template = xmlnode.ReadString( "template" );

        auto idvalue = CalcTypeCodeValue( kfsetting->_type, kfsetting->_code );
        _id_name_list[ idvalue ] = kfsetting->_id;
    }

    uint64 KFRewardConfig::CalcTypeCodeValue( uint64 type, uint64 code )
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFRewardConfig::FormatIntString( const std::string& dataname, uint32 datavalue, uint32 dataid )
    {
        return FormatStrString( dataname, __TO_STRING__( datavalue ), dataid );
    }

    const std::string& KFRewardConfig::FormatStrString( const std::string& dataname, const std::string& datavalue, uint32 dataid )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        auto kfsetting = FindSetting( dataname );
        if ( kfsetting != nullptr )
        {
            _str_element = "[";
            _str_element += __FORMAT__( kfsetting->_template, datavalue, dataid );
            _str_element += "]";
        }
        else
        {
            __LOG_ERROR__( "dataname=[{}] no template", dataname );
        }

        return _str_element;
    }

    const std::string& KFRewardConfig::FormatSettingString( const KFElementSetting& kfsetting )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        auto index = 0u;
        _str_element = "[";
        for ( auto& tupledata : kfsetting._data_list )
        {
            auto& dataname = std::get<0>( tupledata );
            auto& datavalue = std::get<1>( tupledata );
            auto& dataid = std::get<2>( tupledata );

            auto kfrewardsetting = FindSetting( dataname );
            if ( kfrewardsetting == nullptr )
            {
                __LOG_ERROR__( "dataname=[{}] no template", dataname );
                continue;
            }

            if ( index > 0u )
            {
                _str_element += ",";
            }

            ++index;
            _str_element += __FORMAT__( kfrewardsetting->_template, datavalue, dataid );
        }

        _str_element += "]";
        return _str_element;
    }

    const std::string& KFRewardConfig::ParseString( const std::string& strparse )
    {
        static auto rewardtype = KFGlobal::Instance()->GetUInt32( __STRING__( rewardtype ) );
        switch ( rewardtype )
        {
        case KFRewardEnum::CodeJson:
            return ParseCodeString( strparse );
            break;
        }

        return strparse;
    }

    // [1,1,2]
    const std::string& KFRewardConfig::ParseCodeString( const std::string& strparse )
    {
        static std::string _str_element = _invalid_string;
        _str_element.clear();

        // 将字符串解析成数组
        __JSON_PARSE_STRING__( kfjson, strparse );
        if ( !kfjson.IsArray() )
        {
            return _str_element;
        }

        _str_element = "[";
        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( uint32 i = 0u; i < size; ++i )
        {
            auto& jsonarray = __JSON_ARRAY_INDEX__( kfjson, i );
            if ( !jsonarray.IsArray() )
            {
                continue;
            }

            auto len = __JSON_ARRAY_SIZE__( jsonarray );
            if ( len != 3u )
            {
                continue;
            }

            auto type = jsonarray[ 0 ].GetUint();
            auto code = jsonarray[ 1 ].GetUint();
            auto num = jsonarray[ 2 ].GetUint();

            auto& strreward = FormatRewardString( type, code, num );
            if ( strreward.empty() )
            {
                continue;
            }

            if ( i > 0u )
            {
                _str_element += ",";
            }

            _str_element += strreward;
        }
        _str_element += "]";

        return _str_element;
    }


    const std::string& KFRewardConfig::FormatRewardString( uint32 type, uint32 code, uint32 num )
    {
        auto idvalue = CalcTypeCodeValue( type, code );
        auto iter = _id_name_list.find( idvalue );
        if ( iter == _id_name_list.end() )
        {
            return _invalid_string;
        }

        auto kfsetting = FindSetting( iter->second );
        if ( kfsetting == nullptr )
        {
            return _invalid_string;
        }

        static std::string _str_element;
        _str_element = __FORMAT__( kfsetting->_template, num, code );
        return _str_element;
    }

}