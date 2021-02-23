#ifndef __KF_MACROS_H__
#define __KF_MACROS_H__

//Define all macros in this file
#include "fmt/fmt.h"
#include "KFSystem.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef POCO_STATIC
    #define POCO_STATIC
#endif // !1

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FUNC_LINE__
    #define __FUNC_LINE__ __FUNCTION__, __LINE__
#endif

#ifndef __MIN__
    #define __MIN__( x, y ) ( (x) > (y) ? (y) : (x) )
#endif

#ifndef __MAX__
    #define __MAX__( x, y ) ( (x) > (y) ? (x) : (y) )
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NEW_ARRAY__
    #define __NEW_ARRAY__( name, size ) new name[size]
#endif // !__NEW_OBJECT__

#ifndef __DELETE_ARRAY__
    #define __DELETE_ARRAY__( p ) if ( p != nullptr ) { delete[] p; p = nullptr; }
#endif

#ifndef __NEW_OBJECT__
    #define __NEW_OBJECT__( name, ... ) new name( __VA_ARGS__ )
#endif // !__NEW_OBJECT__

#ifndef __DELETE_OBJECT__
    #define __DELETE_OBJECT__( p ) if ( p != nullptr ) { delete p; p = nullptr; }
#endif

#ifndef __SHARED_OBJECT__
#define __SHARED_OBJECT__(name, object) std::shared_ptr<name> object = nullptr;
#endif // !__SHARED_OBJECT__

#ifndef __MAKE_SHARED__
#define __MAKE_SHARED__(name, ...) std::make_shared<name>( __VA_ARGS__ )
#endif // !__MAKE_SHARED__
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TO_STRING__
    #define __TO_STRING__( value ) std::to_string( value )
#endif

#ifndef __TO_INT32__
    #define __TO_INT32__( value ) KFUtility::ToValue<int32>( value )
#endif

#ifndef __TO_UINT32__
    #define __TO_UINT32__( value ) KFUtility::ToValue<uint32>( value )
#endif

#ifndef __TO_INT64__
    #define __TO_INT64__( value ) KFUtility::ToValue<int64>( value )
#endif

#ifndef __TO_UINT64__
    #define __TO_UINT64__( value ) KFUtility::ToValue<uint64>( value )
#endif

#ifndef __TO_DOUBLE__
    #define __TO_DOUBLE__( value ) KFUtility::ToValue<double>( value )
#endif

#ifndef __FORMAT__
    #define __FORMAT__( my_fmt, ... ) fmt::format( my_fmt, __VA_ARGS__ )
#endif

#ifndef __PROTO_TO_MAP__
#define __PROTO_TO_MAP__( pb_data, values )\
    for ( auto iter = pb_data->begin(); iter != pb_data->end(); ++iter )\
    {\
        values[ iter->first ] = iter->second;\
    }
#endif

#ifndef __MAP_TO_PROTO__
#define __MAP_TO_PROTO__( values, pb_data )\
    for ( auto iter = values.begin(); iter != values.end(); ++iter )\
    {\
        pb_data[ iter->first ] = iter->second;\
    }
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __TOP_ONE__
    #define __TOP_ONE__ 1
#endif

#ifndef __TOP_FIVE__
    #define __TOP_FIVE__ 5
#endif

#ifndef __TOP_TEN__
    #define __TOP_TEN__ 10
#endif

#ifndef __MAX_INT8__
    #define __MAX_INT8__ std::numeric_limits<int8>::max()
#endif

#ifndef __MAX_UINT8__
    #define __MAX_UINT8__ std::numeric_limits<uint8>::max()
#endif

#ifndef __MAX_INT16__
    #define __MAX_INT16__ std::numeric_limits<int16>::max()
#endif

#ifndef __MAX_UINT16__
    #define __MAX_UINT16__ std::numeric_limits<uint16>::max()
#endif

#ifndef __MAX_INT32__
    #define __MAX_INT32__ std::numeric_limits<int32>::max()
#endif

#ifndef __MAX_UINT32__
    #define __MAX_UINT32__ std::numeric_limits<uint32>::max()
#endif

#ifndef __MAX_INT64__
    #define __MAX_INT64__ std::numeric_limits<int64>::max()
#endif

#ifndef __MAX_UINT64__
    #define __MAX_UINT64__ std::numeric_limits<uint64>::max()
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
// 空字串
#ifndef __EMPTY_STRING__
    #define __EMPTY_STRING__ _invalid_string
#endif // !__EMPTY_STRING__

// 通配字串
#ifndef __GLOBBING_STRING__
    #define __GLOBBING_STRING__ _globbing_string
#endif // !__EMPTY_STRING__

// 连接符号
#ifndef __SPLIT_STRING__
    #define __SPLIT_STRING__ _split_string
#endif // !__SPLIT_STRING__

// 范围符号
#ifndef __RANGE_STRING__
    #define __RANGE_STRING__ _range_string
#endif

// 域符号
#ifndef __DOMAIN_STRING__
    #define __DOMAIN_STRING__ _domain_string
#endif

// 与符号
#ifndef __AND_STRING__
    #define __AND_STRING__ _and_string
#endif

// 或符号
#ifndef __OR_STRING__
    #define __OR_STRING__ _or_string
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DATABASE_KEY_1__
    #define __DATABASE_KEY_1__( key ) __FORMAT__( "{}", key )
#endif

#ifndef __DATABASE_KEY_2__
    #define __DATABASE_KEY_2__( key1, key2 ) __FORMAT__( "{}:{}", key1, key2 )
#endif

#ifndef __DATABASE_KEY_3__
    #define __DATABASE_KEY_3__( key1, key2, key3 ) __FORMAT__( "{}:{}:{}", key1, key2, key3 )
#endif

#ifndef __DATABASE_KEY_4__
    #define __DATABASE_KEY_4__( key1, key2, key3, key4 ) __FORMAT__( "{}:{}:{}:{}", key1, key2, key3, key4 )
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __MAX_LOOP_COUNT__
    #define __MAX_LOOP_COUNT__ 100000u
#endif

// 有计数上限的安全的do while循环, 防止死循环
#ifndef __DO__
#define __DO__ \
    auto doloopcount = 0u;do
#endif

#ifndef __DO_WHILE__
#define __DO_WHILE__(condition) \
    while( (condition) && (++doloopcount < __MAX_LOOP_COUNT__) );\
    if ( doloopcount >= __MAX_LOOP_COUNT__ )\
    {\
        __LOG_ERROR__( "infinite loop!");\
    }
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
// 有计数上限的安全的while循环, 防止死循环
#ifndef __SAFE_WHILE__
#define __SAFE_WHILE__(condition) \
    auto whileloopcount = 0u;\
    while( (condition) && (++whileloopcount < __MAX_LOOP_COUNT__) );
#endif

#ifndef __SAFE_WHILE_END__
#define __SAFE_WHILE_END__\
    if ( whileloopcount >= __MAX_LOOP_COUNT__ )\
    {\
        __LOG_ERROR__( "infinite loop!");\
    }
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __BETWEEN__
    #define __BETWEEN__(value, min, max) ((value) >= (min) && (value) < (max))
#endif

#endif //!__KF_MACROS_H__