// LinearTransform2D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
LinearTransform2D::LinearTransform2D( void )
{
	Identity();
}

//========================================================================
LinearTransform2D::LinearTransform2D( const LinearTransform2D& transform )
{
	Set( transform );
}

//========================================================================
LinearTransform2D::LinearTransform2D( const Vector2D& xAxis, const Vector2D& yAxis )
{
	Set( xAxis, yAxis );
}

//========================================================================
LinearTransform2D::~LinearTransform2D( void )
{
}

//========================================================================
void LinearTransform2D::Identity( void )
{
	xAxis.Set( 1.0, 0.0 );
	yAxis.Set( 0.0, 1.0 );
}

//========================================================================
void LinearTransform2D::Set( const LinearTransform2D& transform )
{
	xAxis.Set( transform.xAxis );
	yAxis.Set( transform.yAxis );
}

//========================================================================
void LinearTransform2D::Get( LinearTransform2D& transform ) const
{
	xAxis.Get( transform.xAxis );
	yAxis.Get( transform.yAxis );
}

//========================================================================
void LinearTransform2D::Set( const Vector2D& xAxis, const Vector2D& yAxis )
{
	this->xAxis.Set( xAxis );
	this->yAxis.Set( yAxis );
}

//========================================================================
void LinearTransform2D::Get( Vector2D& xAxis, Vector2D& yAxis ) const
{
	this->xAxis.Get( xAxis );
	this->yAxis.Get( yAxis );
}

//========================================================================
Vector2D& LinearTransform2D::SetXAxis( const Vector2D& xAxis )
{
	this->xAxis.Set( xAxis );
	return this->xAxis;
}

//========================================================================
Vector2D& LinearTransform2D::SetYAxis( const Vector2D& yAxis )
{
	this->yAxis.Set( yAxis );
	return this->yAxis;
}

//========================================================================
const Vector2D& LinearTransform2D::GetXAxis( void ) const
{
	return xAxis;
}

//========================================================================
const Vector2D& LinearTransform2D::GetYAxis( void ) const
{
	return yAxis;
}

//========================================================================
Vector2D& LinearTransform2D::GetXAxis( void )
{
	return xAxis;
}

//========================================================================
Vector2D& LinearTransform2D::GetYAxis( void )
{
	return yAxis;
}

//========================================================================
Scalar LinearTransform2D::Determinant( void ) const
{
	Scalar det =
			xAxis.GetX() * yAxis.GetY() -
			xAxis.GetY() * yAxis.GetX();

	return det;
}

//========================================================================
void LinearTransform2D::Transform( Vector2D& result, const Vector2D& vector ) const
{
	Vector2D xComponent, yComponent;

	xComponent.Scale( xAxis, vector.GetX() );
	yComponent.Scale( yAxis, vector.GetY() );

	result.Add( xComponent, yComponent );
}

//========================================================================
void LinearTransform2D::Transform( Vector2D* results, const Vector2D* vectors, unsigned int count ) const
{
	for( unsigned int index = 0; index < count; index++ )
		Transform( results[ index ], vectors[ index ] );
}

//========================================================================
void LinearTransform2D::ComposeRotation( Scalar angle )
{
	Scalar cosAngle = cos( angle );
	Scalar sinAngle = sin( angle );

	xAxis.Set( cosAngle, sinAngle );
	yAxis.Set( -sinAngle, cosAngle );
}

//========================================================================
void LinearTransform2D::ComposeScale( Scalar xScale, Scalar yScale )
{
	xAxis.Set( xScale, 0.0 );
	yAxis.Set( 0.0, yScale );
}

//========================================================================
void LinearTransform2D::ComposeShear( Scalar xyShear )
{
	xAxis.Set( 1.0, 0.0 );
	yAxis.Set( xyShear, 1.0 );
}

//========================================================================
bool LinearTransform2D::Decompose( LinearTransform2D& scale, LinearTransform2D& shear, LinearTransform2D& rotation ) const
{
	return false;
}

//========================================================================
// The only way I know of to formulate this without matrix algebra is to use geometric algebra.
// What you get is a coordinate-free formulation of the inverse a general linear transformation.
bool LinearTransform2D::Invert( LinearTransform2D& result ) const
{
	Scalar det = Determinant();
	if( det == 0.0 )
		return false;

	Scalar recipDet = 1.0 / det;

	result.xAxis.Set( yAxis.GetY(), -xAxis.GetY() ).Scale( recipDet );
	result.yAxis.Set( -yAxis.GetX(), xAxis.GetX() ).Scale( recipDet );

	return true;
}

//========================================================================
bool LinearTransform2D::Invert( void )
{
	LinearTransform2D result;
	if( !Invert( result ) )
		return false;

	Set( result );
	return true;
}

//========================================================================
void LinearTransform2D::OrthogonalInvert( LinearTransform2D& result ) const
{
	Vector2D xAxis( this->xAxis.GetX(), this->yAxis.GetX() );
	Vector2D yAxis( this->xAxis.GetY(), this->yAxis.GetY() );

	result.Set( xAxis, yAxis );
}

//========================================================================
void LinearTransform2D::OrthogonalInvert( void )
{
	LinearTransform2D result;
	OrthogonalInvert( result );
	Set( result );
}

//========================================================================
void LinearTransform2D::Concatinate( const LinearTransform2D& firstTransform, const LinearTransform2D& secondTransform )
{
	Vector2D xAxisTransformed, yAxisTransformed;

	secondTransform.Transform( xAxisTransformed, firstTransform.xAxis );
	secondTransform.Transform( yAxisTransformed, firstTransform.yAxis );

	xAxis.Set( xAxisTransformed );
	yAxis.Set( yAxisTransformed );
}

//========================================================================
bool LinearTransform2D::Solve(
						const Vector2D& preVectorA,
						const Vector2D& preVectorB,
						const Vector2D& postVectorA,
						const Vector2D& postVectorB )
{
	LinearTransform2D transformA, transformB;

	transformA.Set( preVectorA, preVectorB );
	transformB.Set( postVectorA, postVectorB );

	LinearTransform2D transformAInverted;
	if( !transformA.Invert( transformAInverted ) )
		return false;

	Concatinate( transformAInverted, transformB );
	return true;
}

//========================================================================
bool LinearTransform2D::Orthonormalize( void )
{
	// Here we arbitrarily reject the y axis from the x axis.
	Vector2D::OrthoReject( yAxis, xAxis, yAxis );

	if( !xAxis.SetNormalized( xAxis ) )
		return false;

	if( !yAxis.SetNormalized( yAxis ) )
		return false;

	return true;
}

// LinearTransform2D.cpp