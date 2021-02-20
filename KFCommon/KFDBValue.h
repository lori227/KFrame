#ifndef __KF_DB_VALUE_H__
#define __KF_DB_VALUE_H__

#include "KFMap.h"
#include "KFMacros.h"

namespace KFrame
{
    class KFDBValue
    {
    public:
        inline void AddValue( const std::string& key, uint64 value )
        {
            _int_list[ key ] = value;
        }

        inline void AddValue( const std::string& key, const std::string& value, bool is_binary = false )
        {
            if ( is_binary )
            {
                _bin_list[ key ] = value;
            }
            else
            {
                _str_list[ key ] = value;
            }
        }

        inline uint64 FindValue( const std::string& key ) const
        {
            auto iter = _int_list.find( key );
            if ( iter == _int_list.end() )
            {
                return 0u;
            }

            return iter->second;
        }

        inline const std::string& FindStrValue( const std::string& key ) const
        {
            auto iter = _str_list.find( key );
            if ( iter == _str_list.end() )
            {
                iter = _bin_list.find( key );
                if ( iter == _bin_list.end() )
                {
                    return _invalid_string;
                }
            }

            return iter->second;
        }

        bool IsEmpty() const
        {
            return _int_list.empty() && _str_list.empty() && _bin_list.empty();
        }

    public:
        // int 属性列表
        StringUInt64 _int_list;

        // string 属性列表
        StringMap _str_list;

        // binary 属性列表
        StringMap _bin_list;
    };

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
#define __DBVALUE_TO_PROTO__( db_value, pb_value )\
    {\
        auto& int_list = *pb_value->mutable_pbuint64();\
        __MAP_TO_PROTO__( db_value._int_list, int_list);\
        auto& str_list = *pb_value->mutable_pbstring(); \
        __MAP_TO_PROTO__( db_value._str_list, str_list ); \
        __MAP_TO_PROTO__( db_value._bin_list, str_list ); \
    }

#define __PROTO_TO_DBVALUE__( pb_value, db_value  )\
    {\
        auto int_list = &pb_value->pbuint64();\
        __PROTO_TO_MAP__( int_list, db_value._int_list);\
        auto str_list = &pb_value->pbstring(); \
        __PROTO_TO_MAP__( str_list, db_value._str_list  ); \
    }

#define __DBVALUE_TO_MAP__( db_value, map_value )\
    {\
        __MAP_TO_PROTO__( db_value._str_list, map_value ); \
        __MAP_TO_PROTO__( db_value._bin_list, map_value ); \
        for ( auto iter = db_value._int_list.begin(); iter != db_value._int_list.end(); ++iter )\
        {\
            map_value[ iter->first ] = __TO_STRING__(iter->second);\
        }\
    }

#define __MAP_TO_DBVALUE__( map_value, db_value )\
    {\
        __MAP_TO_PROTO__( map_value, db_value._str_list);\
    }

#define __JSON_TO_DBVALUE__( json_value, db_value )\
    for ( auto iter = json_value.MemberBegin(); iter != json_value.MemberEnd(); ++iter )\
    {\
        if ( iter->value.IsString() )\
        {\
            db_value.AddValue(iter->name.GetString(), iter->value.GetString());\
        }\
        else if ( iter->value.IsNumber() )\
        {\
            db_value.AddValue(iter->name.GetString(), iter->value.GetUint64());\
        }\
    }
}
#endif

