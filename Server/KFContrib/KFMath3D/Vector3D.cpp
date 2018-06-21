// Vector3D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
const Vector3D Vector3D::xAxis( 1.0, 0.0, 0.0 );
const Vector3D Vector3D::yAxis( 0.0, 1.0, 0.0 );
const Vector3D Vector3D::zAxis( 0.0, 0.0, 1.0 );

//========================================================================
Vector3D::Vector3D( void )
{
	Set( 0.0, 0.0, 0.0 );
}

//========================================================================
Vector3D::Vector3D( const Vector3D& vector )
{
	Set( vector );
}

//========================================================================
Vector3D::Vector3D( Scalar x, Scalar y, Scalar z )
{
	Set( x, y, z );
}

//========================================================================
Vector3D::~Vector3D( void )
{
}

//========================================================================
Vector3D& Vector3D::Set( const Vector3D& vector )
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	return *this;
}

//========================================================================
void Vector3D::Get( Vector3D& result ) const
{
	result.x = x;
	result.y = y;
	result.z = z;
}

//========================================================================
Vector3D& Vector3D::Set( Scalar x, Scalar y, Scalar z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

//========================================================================
void Vector3D::Get( Scalar& x, Scalar& y, Scalar& z ) const
{
	x = this->x;
	y = this->y;
	z = this->z;
}

//========================================================================
void Vector3D::SetX( Scalar x )
{
	this->x = x;
}

//========================================================================
void Vector3D::SetY( Scalar y )
{
	this->y = y;
}

//========================================================================
void Vector3D::SetZ( Scalar z )
{
	this->z = z;
}

//========================================================================
Scalar Vector3D::GetX( void ) const
{
	return x;
}

//========================================================================
Scalar Vector3D::GetY( void ) const
{
	return y;
}

//========================================================================
Scalar Vector3D::GetZ( void ) const
{
	return z;
}

//========================================================================
Scalar Vector3D::Length( void ) const
{
	Scalar squareMag = Dot( *this, *this );
	Scalar mag = sqrt( squareMag );
	return mag;
}

//========================================================================
bool Vector3D::SetNormalized( const Vector3D& vector )
{
	Scalar mag = vector.Length();
	if( mag == 0.0 )
		return false;

	Scalar recipMag = 1.0 / mag;
	Scale( *this, *this, recipMag );
	return true;
}

//========================================================================
bool Vector3D::GetNormalized( Vector3D& result ) const
{
	return result.SetNormalized( *this );
}

//========================================================================
Scalar Vector3D::Angle( const Vector3D& vector ) const
{
	Vector3D thisNormalized, givenNormalized;
	thisNormalized.SetNormalized( *this );
	givenNormalized.SetNormalized( vector );

	Scalar dot = Dot( thisNormalized, givenNormalized );
	Scalar angle = acos( dot );
	return angle;
}

//========================================================================
Vector3D& Vector3D::operator=( const Vector3D& vector )
{
	Set( vector );
	return *this;
}

//========================================================================
Vector3D Vector3D::operator+( const Vector3D& vector ) const
{
	Vector3D result;
	Add( result, *this, vector );
	return result;
}

//========================================================================
Vector3D Vector3D::operator-( const Vector3D& vector ) const
{
	Vector3D result;
	Sub( result, *this, vector );
	return result;
}

//========================================================================
Vector3D Vector3D::operator*( const Vector3D& vector ) const
{
	Vector3D result;
	Cross( result, *this, vector );
	return result;
}

//========================================================================
Vector3D Vector3D::operator*( Scalar scalar ) const
{
	Vector3D result;
	Scale( result, *this, scalar );
	return result;
}

//========================================================================
void Vector3D::operator+=( const Vector3D& vector )
{
	Add( vector );
}

//========================================================================
void Vector3D::operator-=( const Vector3D& vector )
{
	Sub( vector );
}

//========================================================================
Vector3D& Vector3D::Add( const Vector3D& vectorA, const Vector3D& vectorB )
{
	Add( *this, vectorA, vectorB );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Add( const Vector3D& vectorB )
{
	Add( *this, *this, vectorB );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Sub( const Vector3D& vectorA, const Vector3D& vectorB )
{
	Sub( *this, vectorA, vectorB );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Sub( const Vector3D& vectorB )
{
	Sub( *this, *this, vectorB );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Cross( const Vector3D& vectorA, const Vector3D& vectorB )
{
	Cross( *this, vectorA, vectorB );
	return *this;
}

//========================================================================
Scalar Vector3D::Dot( const Vector3D& vector ) const
{
	Scalar dot = Dot( *this, vector );
	return dot;
}

//========================================================================
Vector3D& Vector3D::Scale( Scalar scalar )
{
	Scale( *this, *this, scalar );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Scale( const Vector3D& vector, Scalar scalar )
{
	Scale( *this, vector, scalar );
	return *this;
}

//========================================================================
Vector3D& Vector3D::OrthoProject( const Vector3D& projectionVector, const Vector3D& vector )
{
	OrthoProject( *this, projectionVector, vector );
	return *this;
}

//========================================================================
Vector3D& Vector3D::OrthoReject( const Vector3D& rejectionVector, const Vector3D& vector )
{
	OrthoReject( *this, rejectionVector, vector );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Rotate( const Vector3D& unitAxis, Scalar angle )
{
	Rotate( *this, *this, unitAxis, angle );
	return *this;
}

//========================================================================
Vector3D& Vector3D::Rotate( const Vector3D& vector, const Vector3D& unitAxis, Scalar angle )
{
	Rotate( *this, vector, unitAxis, angle );
	return *this;
}

//========================================================================
/*static*/ void Vector3D::Add( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB )
{
	result.x = vectorA.x + vectorB.x;
	result.y = vectorA.y + vectorB.y;
	result.z = vectorA.z + vectorB.z;
}

//========================================================================
/*static*/ void Vector3D::Sub( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB )
{
	result.x = vectorA.x - vectorB.x;
	result.y = vectorA.y - vectorB.y;
	result.z = vectorA.z - vectorB.z;
}

//========================================================================
/*static*/ void Vector3D::Cross( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB )
{
	Scalar x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	Scalar y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	Scalar z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;

	result.Set( x, y, z );
}

//========================================================================
/*static*/ Scalar Vector3D::Dot( const Vector3D& vectorA, const Vector3D& vectorB )
{
	Scalar dot =
			vectorA.x * vectorB.x +
			vectorA.y * vectorB.y +
			vectorA.z * vectorB.z;

	return dot;
}

//========================================================================
/*static*/ void Vector3D::Scale( Vector3D& result, const Vector3D& vector, Scalar scalar )
{
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	result.z = vector.z * scalar;
}

//========================================================================
/*static*/ void Vector3D::OrthoProject( Vector3D& result, const Vector3D& unitProjectionVector, const Vector3D& vector )
{
	Scalar dot = Dot( vector, unitProjectionVector );
	Scale( result, unitProjectionVector, dot );
}

//========================================================================
/*static*/ void Vector3D::OrthoReject( Vector3D& result, const Vector3D& unitRejectionVector, const Vector3D& vector )
{
	Vector3D orthoProjection;
	OrthoProject( orthoProjection, unitRejectionVector, vector );
	Sub( result, vector, orthoProjection );
}

//========================================================================
/*static*/ void Vector3D::Rotate( Vector3D& result, const Vector3D& vector, const Vector3D& unitAxis, Scalar angle )
{
	Vector3D projectedComponent, rejectedComponent;

	Scalar dot = Dot( vector, unitAxis );
	Scale( projectedComponent, unitAxis, dot );
	Sub( rejectedComponent, vector, projectedComponent );

	// I know there's a way to do this without a branch, but this works.
	Scalar length = rejectedComponent.Length();
	if( length != 0.0 )
	{
		Vector3D xAxis, yAxis;
		xAxis.Scale( rejectedComponent, 1.0 / length );
		yAxis.Cross( unitAxis, xAxis );

		Scalar sinAngle, cosAngle;
		sinAngle = sin( angle );
		cosAngle = cos( angle );

		rejectedComponent = xAxis * cosAngle + yAxis * sinAngle;
	}

	result.Add( projectedComponent, rejectedComponent );
}

//========================================================================
/*static*/ void Vector3D::Rotation( Vector3D& unitAxis, Scalar& angle, const Vector3D& vectorA, const Vector3D& vectorB )
{
	angle = vectorA.Angle( vectorB );
	unitAxis.Cross( vectorA, vectorB );
	unitAxis.SetNormalized( unitAxis );
}

// Vector3D.cpp