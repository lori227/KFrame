#ifndef __KF_JSON_H__
#define __KF_JSON_H__

#include "KFInclude.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace KFrame
{
    typedef rapidjson::Document KFJson;

    // 序列化
    static inline std::string JsonSerialize( rapidjson::Document& kfjson )
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer( buffer );
        kfjson.Accept( writer );

        return buffer.GetString();
    }

    // 获得数字
    template< class T >
    static inline T GetJsonNumber( rapidjson::Value& kfjson, std::string& name )
    {
        auto& value = kfjson[ name ];
        if ( value.IsNumber() )
        {
            return value.Get< T >();
        }
        else if ( value.IsString() )
        {
            return static_cast< T >( atoll( value.GetString() ) );
        }

        return 0;
    }

    // 获得字串
    static inline std::string GetJsonString( rapidjson::Value& kfjson, std::string& name )
    {
        auto& value = kfjson[ name ];
        if ( value.IsString() )
        {
            return value.GetString();
        }
        else if ( value.IsNumber() )
        {
            return std::to_string( value.GetUint64() );
        }

        return "";
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // 解析json
#define __JSON_PARSE_STRING__( kfjson, data )\
    rapidjson::Document kfjson;\
    kfjson.Parse( data.c_str() );\
    if ( kfjson.HasParseError() )\
    {\
        __LOG_ERROR__( "json=[{}] parse error=[{}]!", data, kfjson.GetParseError() );\
    }\

#define __JSON_PARSE_CHAR__( kfjson, data, size )\
    rapidjson::Document kfjson;\
    kfjson.Parse( data, size );\
    if ( kfjson.HasParseError() )\
    {\
        __LOG_ERROR__( "json=[{}] parse error=[{}]!", data, kfjson.GetParseError() ); \
    }\

#define __JSON_SERIALIZE__( kfjson ) JsonSerialize( kfjson )

#define  __JSON_DOCUMENT__( kfjson )\
    rapidjson::Document kfjson;\
    auto &allocator = kfjson.GetAllocator();\
    kfjson.SetObject();\

#define  __JSON_OBJECT__( kfjson )\
    rapidjson::Value kfjson( rapidjson::kObjectType )

#define  __JSON_ARRAY__( kfjson )\
    rapidjson::Value kfjson( rapidjson::kArrayType )

#define __JSON_GET_INT32__( kfjson, name ) GetJsonNumber< int32 >( kfjson, name )
#define __JSON_GET_UINT32__( kfjson, name ) GetJsonNumber< uint32 >( kfjson, name )
#define __JSON_GET_INT64__( kfjson, name ) GetJsonNumber< int64 >( kfjson, name )
#define __JSON_GET_UINT64__( kfjson, name ) GetJsonNumber< uint64 >( kfjson, name )
#define __JSON_GET_FLOAT__( kfjson, name ) GetJsonNumber< float >( kfjson, name )
#define __JSON_GET_DOUBLE__( kfjson, name ) GetJsonNumber< double >( kfjson, name )
#define __JSON_GET_STRING__( kfjson, name ) GetJsonString( kfjson, name )
#define __JSON_GET_OBJECT__( kfjson, name ) kfjson[ name ]
#define __JSON_GET_ARRRY__( kfjson, name ) kfjson[ name ].GetArray()

#define __JSON_SET_VALUE__( kfjson, name, value ) kfjson.AddMember( rapidjson::StringRef( name.c_str() ), value, allocator )

#define  __JSON_ADD_VALUE__( kfjson, value ) kfjson.PushBack( value, allocator );
#define  __JSON_ADD_STRING__( kfjson, value ) kfjson.PushBack( rapidjson::StringRef( value.c_str() ), allocator )

#define __JSON_HAS_MEMBER__( kfjson, name ) kfjson.HasMember( name )
#define __JOSN_REMOVE__( kfjson, name ) kfjson.RemoveMember( name )

#define  __JSON_ARRAY_SIZE__( kfjson ) kfjson.Size()
#define  __JSON_ARRAY_INDEX__( kfjson, index ) kfjson[ index ]

#define  __JSON_FROM_MAP__( kfjson, values ) \
    for( auto& iter : values )\
    {\
        auto& key = iter.first;\
        auto& value = iter.second;\
        __JSON_SET_VALUE__( kfjson, key, value );\
    }\

#define  __JSON_TO_MAP__( kfjson, values ) \
    for( auto iter = kfjson.MemberBegin(); iter != kfjson.MemberEnd(); ++iter )\
    {\
        std::string value = "";\
        if ( iter->value.IsString() )\
        {   \
            value = iter->value.GetString(); \
        }\
        else if ( iter->value.IsNumber() )\
        {   \
            value = std::to_string( iter->value.GetUint64() ); \
        }\
        values[ iter->name.GetString() ] = value; \
    }\


}

#endif
