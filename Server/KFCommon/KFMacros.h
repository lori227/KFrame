#ifndef __KF_MACROS_H__
#define __KF_MACROS_H__

//Define all macros in this file

#include "KFSystem.h"
#include "spdlog/fmt/fmt.h"
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef POCO_STATIC
    #define POCO_STATIC
#endif // !1

#ifndef __FUNCTION_LINE__
    #define __FUNCTION_LINE__ __FUNCTION__, __LINE__
#endif

#ifndef __MIN__
    #define __MIN__( x, y ) ( (x) > (y) ? (y) : (x) )
#endif

#ifndef __MAX__
    #define __MAX__( x, y ) ( (x) > (y) ? (x) : (y) )
#endif

#ifndef __NEW_ARRAY__
    #define __NEW_ARRAY__( name, size ) new name[size]
#endif // !__NEW_OBJECT__

#ifndef __DELETE_ARRAY__
    #define __DELETE_ARRAY__( p ) if ( p != nullptr ) { delete[] p; p = nullptr; }
#endif

#ifndef __NEW_OBJECT__
    #define __NEW_OBJECT__( name ) new name()
#endif // !__NEW_OBJECT__

#ifndef __DELETE_OBJECT__
    #define __DELETE_OBJECT__( p ) if ( p != nullptr ) { delete p; p = nullptr; }
#endif

#ifndef __TO_STRING__
    #define __TO_STRING__( value ) std::to_string( value )
#endif

#ifndef __TO_CHAR__
    #define __TO_CHAR__( value ) std::to_string( value ).c_str()
#endif


enum ELogCategory
{
    ELC_NONE = 0,
    ELC_SYSTEM,
    ELC_INIT,
    ELC_LOGIC,
    ELC_NET,
    ELC_SQL,
    ELC_MEMORY,
    ELC_SCRIPT,
    ELC_LOGIN,
    ELC_PLAYER,
    //TO ADD
    ELC_MAX,
};

static const std::string LogCategoryName[] =
{
    "none",
    "system",
    "init",
    "logic",
    "net",
    "sql",
    "memory",
    "script",
    "login",
    "player",
    //TO ADD
};

#define KF_FORMAT(my_fmt, ...)				fmt::format(my_fmt, ##__VA_ARGS__);
#define KF_FORMAT_FUNCTION(my_fmt, ...)		fmt::format(std::string("[{}:{}]") + my_fmt, __FUNCTION_LINE__, ##__VA_ARGS__);
//////////////////////////////////////////////////////////////////////////////////////////////////


#endif //!__KF_MACROS_H__