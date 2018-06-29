// MathLibrary.h

#pragma once

//========================================================================
#if defined MATH_LIBRARY_EXPORT
    #define MATH_LIBRARY_API		__declspec( dllexport )
#elif defined MATH_LIBRARY_IMPORT
    #define MATH_LIBRARY_API		__declspec( dllimport )
#else
    #define MATH_LIBRARY_API
#endif

//========================================================================
#include <math.h>

//========================================================================
namespace Math3D
{
    typedef double Scalar;

    class Vector2D;
    class Vector3D;
    class LinearTransform2D;
    class LinearTransform3D;
    class AffineTransform2D;
    class AffineTransform3D;
}

//========================================================================
#include "Vector2D.h"
#include "Vector3D.h"
#include "LinearTransform2D.h"
#include "LinearTransform3D.h"
#include "AffineTransform2D.h"
#include "AffineTransform3D.h"

// MathLibrary.h