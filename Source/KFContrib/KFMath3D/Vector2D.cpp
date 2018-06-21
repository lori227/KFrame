// Vector2D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
const Vector2D Vector2D::xAxis( 1.0, 0.0 );
const Vector2D Vector2D::yAxis( 0.0, 1.0 );

//========================================================================
Vector2D::Vector2D( void )
{
	Set( 0.0, 0.0 );
}

//========================================================================
Vector2D::Vector2D( const Vector2D& vector )
{
	Set( vector );
}

//========================================================================
Vector2D::Vector2D( Scalar x, Scalar y )
{
	Set( x, y );
}

//========================================================================
Vector2D::~Vector2D( void )
{
}

//========================================================================
Vector2D& Vector2D::Set( const Vector2D& vector )
{
	x = vector.x;
	y = vector.y;
	return *this;
}

//========================================================================
void Vector2D::Get( Vector2D& result ) const
{
	result.x = x;
	result.y = y;
}

//========================================================================
Vector2D& Vector2D::Set( Scalar x, Scalar y )
{
	this->x = x;
	this->y = y;
	return *this;
}

//========================================================================
void Vector2D::Get( Scalar& x, Scalar& y ) const
{
	x = this->x;
	y = this->y;
}

//========================================================================
void Vector2D::SetX( Scalar x )
{
	this->x = x;
}

//========================================================================
void Vector2D::SetY( Scalar y )
{
	this->y = y;
}

//========================================================================
Scalar Vector2D::GetX( void ) const
{
	return x;
}

//========================================================================
Scalar Vector2D::GetY( void ) const
{
	return y;
}

//========================================================================
Scalar Vector2D::Length( void ) const
{
	Scalar squareMag = Dot( *this, *this );
	Scalar mag = sqrt( squareMag );
	return mag;
}

//========================================================================
bool Vector2D::SetNormalized( const Vector2D& vector )
{
	Scalar mag = vector.Length();
	if( mag == 0.0 )
		return false;

	Scalar recipMag = 1.0 / mag;
	Scale( *this, *this, recipMag );
	return true;
}

//========================================================================
bool Vector2D::GetNormalized( Vector2D& result ) const
{
	return result.SetNormalized( *this );
}

//========================================================================
Scalar Vector2D::Angle( const Vector2D& vector ) const
{
	Vector2D thisNormalized, givenNormalized;
	thisNormalized.SetNormalized( *this );
	givenNormalized.SetNormalized( vector );

	Scalar dot = Dot( thisNormalized, givenNormalized );
	Scalar angle = acos( dot );
	return angle;
}

//========================================================================
Vector2D& Vector2D::operator=( const Vector2D& vector )
{
	Set( vector.x, vector.y );
	return *this;
}

//========================================================================
Vector2D Vector2D::operator+( const Vector2D& vector ) const
{
	Vector2D result;
	Add( result, *this, vector );
	return result;
}

//========================================================================
Vector2D Vector2D::operator-( const Vector2D& vector ) const
{
	Vector2D result;
	Sub( result, *this, vector );
	return result;
}

//========================================================================
Vector2D Vector2D::operator*( Scalar scalar ) const
{
	Vector2D result;
	Scale( result, *this, scalar );
	return result;
}


//========================================================================
void Vector2D::operator+=( const Vector2D& vector )
{
	Add( vector );
}

//========================================================================
void Vector2D::operator-=( const Vector2D& vector )
{
	Sub( vector );
}

//========================================================================
Vector2D& Vector2D::Add( const Vector2D& vectorA, const Vector2D& vectorB )
{
	Add( *this, vectorA, vectorB );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Add( const Vector2D& vectorB )
{
	Add( *this, *this, vectorB );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Sub( const Vector2D& vectorA, const Vector2D& vectorB )
{
	Sub( *this, vectorA, vectorB );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Sub( const Vector2D& vectorB )
{
	Sub( *this, *this, vectorB );
	return *this;
}

//========================================================================
Scalar Vector2D::Dot( const Vector2D& vector ) const
{
	Scalar dot = Dot( *this, vector );
	return dot;
}

//========================================================================
Vector2D& Vector2D::Scale( Scalar scalar )
{
	Scale( *this, *this, scalar );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Scale( const Vector2D& vector, Scalar scalar )
{
	Scale( *this, vector, scalar );
	return *this;
}

//========================================================================
Vector2D& Vector2D::OrthoProject( const Vector2D& unitProjectionVector, const Vector2D& vector )
{
	OrthoProject( *this, unitProjectionVector, vector );
	return *this;
}

//========================================================================
Vector2D& Vector2D::OrthoReject( const Vector2D& unitRejectionVector, const Vector2D& vector )
{
	OrthoReject( *this, unitRejectionVector, vector );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Rotate( Scalar angle )
{
	Rotate( *this, *this, angle );
	return *this;
}

//========================================================================
Vector2D& Vector2D::Rotate( const Vector2D& vector, Scalar angle )
{
	Rotate( *this, vector, angle );
	return *this;
}

//========================================================================
/*static*/ void Vector2D::Add( Vector2D& result, const Vector2D& vectorA, const Vector2D& vectorB )
{
	result.x = vectorA.x + vectorB.x;
	result.y = vectorA.y + vectorB.y;
}

//========================================================================
/*static*/ void Vector2D::Sub( Vector2D& result, const Vector2D& vectorA, const Vector2D& vectorB )
{
	result.x = vectorA.x - vectorB.x;
	result.y = vectorA.y - vectorB.y;
}

//========================================================================
/*static*/ Scalar Vector2D::Dot( const Vector2D& vectorA, const Vector2D& vectorB )
{
	Scalar dot = vectorA.x * vectorB.x + vectorA.y * vectorB.y;
	return dot;
}

//========================================================================
/*static*/ void Vector2D::Scale( Vector2D& result, const Vector2D& vector, Scalar scalar )
{
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
}

//========================================================================
/*static*/ void Vector2D::OrthoProject( Vector2D& result, const Vector2D& unitProjectionVector, const Vector2D& vector )
{
	Scalar dot = Dot( vector, unitProjectionVector );
	Scale( result, unitProjectionVector, dot );
}

//========================================================================
/*static*/ void Vector2D::OrthoReject( Vector2D& result, const Vector2D& unitRejectionVector, const Vector2D& vector )
{
	Vector2D orthoProjection;
	OrthoProject( orthoProjection, unitRejectionVector, vector );
	Sub( result, vector, orthoProjection );
}

//========================================================================
/*static*/ void Vector2D::Rotate( Vector2D& result, const Vector2D& vector, Scalar angle )
{
	LinearTransform2D rotation;
	rotation.ComposeRotation( angle );
	rotation.Transform( result, vector );
}

// Vector2D.cpp