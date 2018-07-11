#ifndef __KF_MACROS_H__
#define __KF_MACROS_H__

//Define all macros in this file

#include "KFSystem.h"
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

#ifndef __DELETE_ARRAY__
    #define __DELETE_ARRAY__( p ) if ( p != nullptr ) { delete[] p; p = nullptr; }
#endif

#ifndef __DELETE_OBJECT__
    #define __DELETE_OBJECT__( p ) if ( p != nullptr ) { delete p; p = nullptr; }
#endif

#ifndef __KF_STRING__
    #define __KF_STRING__( value ) std::to_string( value )
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////


#endif //!__KF_MACROS_H__