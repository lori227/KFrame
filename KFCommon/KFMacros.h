#ifndef __KF_MACROS_H__
#define __KF_MACROS_H__

//Define all macros in this file

#include "KFSystem.h"
#include "fmt/fmt.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef POCO_STATIC
    #define POCO_STATIC
#endif // !1

#ifndef __FUNC_LINE__
    #define __FUNC_LINE__ __FUNCTION__, __LINE__
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

#ifndef __FORMAT__
    #define __FORMAT__( myfmt, ... ) fmt::format( fmt(myfmt), ##__VA_ARGS__ )
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


#endif //!__KF_MACROS_H__