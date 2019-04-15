// LinearTransform3D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
LinearTransform3D::LinearTransform3D( void )
{
    Identity();
}

//========================================================================
LinearTransform3D::LinearTransform3D( const LinearTransform3D& transform )
{
    Set( transform );
}

//========================================================================
LinearTransform3D::LinearTransform3D( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis )
{
    Set( xAxis, yAxis, zAxis );
}

//========================================================================
LinearTransform3D::~LinearTransform3D( void )
{
}

//========================================================================
void LinearTransform3D::Identity( void )
{
    xAxis = Vector3D::xAxis;
    yAxis = Vector3D::yAxis;
    zAxis = Vector3D::zAxis;
}

//========================================================================
void LinearTransform3D::Set( const LinearTransform3D& transform )
{
    xAxis = transform.xAxis;
    yAxis = transform.yAxis;
    zAxis = transform.zAxis;
}

//========================================================================
void LinearTransform3D::Get( LinearTransform3D& transform ) const
{
    transform.xAxis = xAxis;
    transform.yAxis = yAxis;
    transform.zAxis = zAxis;
}

//========================================================================
void LinearTransform3D::Set( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis )
{
    this->xAxis = xAxis;
    this->yAxis = yAxis;
    this->zAxis = zAxis;
}

//========================================================================
void LinearTransform3D::Get( Vector3D& xAxis, Vector3D& yAxis, Vector3D& zAxis ) const
{
    xAxis = this->xAxis;
    yAxis = this->yAxis;
    zAxis = this->zAxis;
}

//========================================================================
Vector3D& LinearTransform3D::SetXAxis( const Vector3D& xAxis )
{
    this->xAxis = xAxis;
    return this->xAxis;
}

//========================================================================
Vector3D& LinearTransform3D::SetYAxis( const Vector3D& yAxis )
{
    this->yAxis = yAxis;
    return this->yAxis;
}

//========================================================================
Vector3D& LinearTransform3D::SetZAxis( const Vector3D& zAxis )
{
    this->zAxis = zAxis;
    return this->zAxis;
}

//========================================================================
const Vector3D& LinearTransform3D::GetXAxis( void ) const
{
    return xAxis;
}

//========================================================================
const Vector3D& LinearTransform3D::GetYAxis( void ) const
{
    return yAxis;
}

//========================================================================
const Vector3D& LinearTransform3D::GetZAxis( void ) const
{
    return zAxis;
}

//========================================================================
Vector3D& LinearTransform3D::GetXAxis( void )
{
    return xAxis;
}

//========================================================================
Vector3D& LinearTransform3D::GetYAxis( void )
{
    return yAxis;
}

//========================================================================
Vector3D& LinearTransform3D::GetZAxis( void )
{
    return zAxis;
}

//========================================================================
Scalar LinearTransform3D::Determinant( void ) const
{
    Scalar subDet0 =
        yAxis.GetX() * zAxis.GetY() -
        zAxis.GetX() * yAxis.GetY();

    Scalar subDet1 =
        yAxis.GetY() * zAxis.GetZ() -
        zAxis.GetY() * yAxis.GetZ();

    Scalar subDet2 =
        yAxis.GetX() * zAxis.GetZ() -
        zAxis.GetX() * yAxis.GetZ();

    Scalar det =
        xAxis.GetX() * subDet1 -
        xAxis.GetY() * subDet2 +
        xAxis.GetZ() * subDet0;

    return det;
}

//========================================================================
void LinearTransform3D::Transform( Vector3D& result, const Vector3D& vector ) const
{
    Vector3D xComponent, yComponent, zComponent;

    xComponent.Scale( xAxis, vector.GetX() );
    yComponent.Scale( yAxis, vector.GetY() );
    zComponent.Scale( zAxis, vector.GetZ() );

    result.Add( xComponent, yComponent ).Add( zComponent );
}

//========================================================================
void LinearTransform3D::Transform( Vector3D* results, const Vector3D* vectors, unsigned int count ) const
{
    for ( unsigned int index = 0; index < count; index++ )
        Transform( results[ index ], vectors[ index ] );
}

//========================================================================
void LinearTransform3D::ComposeRotation( const Vector3D& unitAxis, Scalar angle )
{
    xAxis.Rotate( Vector3D::xAxis, unitAxis, angle );
    yAxis.Rotate( Vector3D::yAxis, unitAxis, angle );
    zAxis.Rotate( Vector3D::zAxis, unitAxis, angle );
}

//========================================================================
void LinearTransform3D::ComposeScale( Scalar xScale, Scalar yScale, Scalar zScale )
{
    xAxis.Scale( Vector3D::xAxis, xScale );
    yAxis.Scale( Vector3D::yAxis, yScale );
    zAxis.Scale( Vector3D::zAxis, zScale );
}

//========================================================================
void LinearTransform3D::ComposeShear( Scalar xyShear, Scalar yzShear, Scalar xzShear )
{
    xAxis.Set( 1.0, 0.0, 0.0 );
    yAxis.Set( xyShear, 1.0, 0.0 );
    zAxis.Set( xzShear, yzShear, 1.0 );
}

//========================================================================
bool LinearTransform3D::Decompose( LinearTransform3D& scale, LinearTransform3D& shear, LinearTransform3D& rotation ) const
{
    return false;
}

//========================================================================
bool LinearTransform3D::Invert( LinearTransform3D& result ) const
{
    Scalar det = Determinant();
    if ( det == 0.0 )
        return false;

    Scalar recipDet = 1.0 / det;

    result.xAxis.Set(
        yAxis.GetY() * zAxis.GetZ() - yAxis.GetZ() * zAxis.GetY(),
        zAxis.GetY() * xAxis.GetZ() - zAxis.GetZ() * xAxis.GetY(),
        xAxis.GetY() * yAxis.GetZ() - xAxis.GetZ() * yAxis.GetY() ).Scale( recipDet );

    result.yAxis.Set(
        yAxis.GetZ() * zAxis.GetX() - yAxis.GetX() * zAxis.GetZ(),
        zAxis.GetZ() * xAxis.GetX() - zAxis.GetX() * xAxis.GetZ(),
        xAxis.GetZ() * yAxis.GetX() - xAxis.GetX() * yAxis.GetZ() ).Scale( recipDet );

    result.zAxis.Set(
        yAxis.GetX() * zAxis.GetY() - yAxis.GetY() * zAxis.GetX(),
        zAxis.GetX() * xAxis.GetY() - zAxis.GetY() * xAxis.GetX(),
        xAxis.GetX() * yAxis.GetY() - xAxis.GetY() * yAxis.GetX() ).Scale( recipDet );

    return true;
}

//========================================================================
bool LinearTransform3D::Invert( void )
{
    LinearTransform3D result;
    if ( !Invert( result ) )
        return false;

    Set( result );
    return true;
}

//========================================================================
void LinearTransform3D::OrthogonalInvert( LinearTransform3D& result ) const
{
    result.xAxis.Set( xAxis.GetX(), yAxis.GetX(), zAxis.GetX() );
    result.yAxis.Set( xAxis.GetY(), yAxis.GetY(), zAxis.GetY() );
    result.zAxis.Set( xAxis.GetZ(), yAxis.GetZ(), zAxis.GetZ() );
}

//========================================================================
void LinearTransform3D::OrthogonalInvert( void )
{
    LinearTransform3D result;
    OrthogonalInvert( result );
    Set( result );
}

//========================================================================
void LinearTransform3D::Concatinate( const LinearTransform3D& firstTransform, const LinearTransform3D& secondTransform )
{
    Vector3D xAxisTransformed, yAxisTransformed, zAxisTransformed;

    secondTransform.Transform( xAxisTransformed, firstTransform.xAxis );
    secondTransform.Transform( yAxisTransformed, firstTransform.yAxis );
    secondTransform.Transform( zAxisTransformed, firstTransform.zAxis );

    xAxis.Set( xAxisTransformed );
    yAxis.Set( yAxisTransformed );
    zAxis.Set( zAxisTransformed );
}

//========================================================================
bool LinearTransform3D::Solve(
    const Vector3D& preVectorA,
    const Vector3D& preVectorB,
    const Vector3D& preVectorC,
    const Vector3D& postVectorA,
    const Vector3D& postVectorB,
    const Vector3D& postVectorC )
{
    LinearTransform3D transformA, transformB;

    transformA.Set( preVectorA, preVectorB, preVectorC );
    transformB.Set( postVectorA, postVectorB, postVectorC );

    LinearTransform3D transformAInverted;
    if ( !transformA.Invert( transformAInverted ) )
        return false;

    Concatinate( transformAInverted, transformB );
    return true;
}

//========================================================================
bool LinearTransform3D::Orthonormalize( void )
{
    // Here we arbitrarily reject the y ans z axes from the x axis.
    Vector3D::OrthoReject( yAxis, xAxis, yAxis );
    Vector3D::OrthoReject( zAxis, xAxis, zAxis );

    if ( !xAxis.SetNormalized( xAxis ) )
        return false;

    if ( !yAxis.SetNormalized( yAxis ) )
        return false;

    if ( !zAxis.SetNormalized( zAxis ) )
        return false;

    return true;
}

// LinearTransform3D.cpp