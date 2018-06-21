// AffineTransform3D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
AffineTransform3D::AffineTransform3D( void )
{
	Identity();
}

//========================================================================
AffineTransform3D::AffineTransform3D( const AffineTransform3D& transform )
{
	Set( transform );
}

//========================================================================
AffineTransform3D::AffineTransform3D( const LinearTransform3D& linearTransform, const Vector3D& translation )
{
	Set( linearTransform, translation );
}

//========================================================================
AffineTransform3D::AffineTransform3D( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis, const Vector3D& translation )
{
	Set( xAxis, yAxis, zAxis, translation );
}

//========================================================================
AffineTransform3D::~AffineTransform3D( void )
{
}

//========================================================================
void AffineTransform3D::Identity( void )
{
	linearTransform.Identity();
	translationVector.Set( 0.0, 0.0, 0.0 );
}

//========================================================================
void AffineTransform3D::Set( const AffineTransform3D& transform )
{
	linearTransform.Set( transform.linearTransform );
	translationVector.Set( transform.translationVector );
}

//========================================================================
void AffineTransform3D::Get( AffineTransform3D& transform ) const
{
	linearTransform.Get( transform.linearTransform );
	translationVector.Get( transform.translationVector );
}

//========================================================================
void AffineTransform3D::Set( const LinearTransform3D& linearTransform, const Vector3D& translation )
{
	this->linearTransform.Set( linearTransform );
	translationVector.Set( translation );
}

//========================================================================
void AffineTransform3D::Get( LinearTransform3D& linearTransform, Vector3D& translation ) const
{
	this->linearTransform.Get( linearTransform );
	translation.Set( translationVector );
}

//========================================================================
void AffineTransform3D::Set( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis, const Vector3D& translation )
{
	linearTransform.Set( xAxis, yAxis, zAxis );
	translationVector.Set( translation );
}

//========================================================================
void AffineTransform3D::Get( Vector3D& xAxis, Vector3D& yAxis, Vector3D& zAxis, Vector3D& translation ) const
{
	linearTransform.Get( xAxis, yAxis, zAxis );
	translationVector.Get( translation );
}

//========================================================================
void AffineTransform3D::SetXAxis( const Vector3D& xAxis )
{
	linearTransform.SetXAxis( xAxis );
}

//========================================================================
void AffineTransform3D::SetYAxis( const Vector3D& yAxis )
{
	linearTransform.SetYAxis( yAxis );
}

//========================================================================
void AffineTransform3D::SetZAxis( const Vector3D& zAxis )
{
	linearTransform.SetZAxis( zAxis );
}

//========================================================================
void AffineTransform3D::SetTranslation( const Vector3D& translation )
{
	translationVector.Set( translation );
}

//========================================================================
void AffineTransform3D::SetTranslation( Scalar x, Scalar y, Scalar z )
{
	translationVector.Set( x, y, z );
}

//========================================================================
void AffineTransform3D::SetLinearTransform( const LinearTransform3D& linearTransform )
{
	this->linearTransform.Set( linearTransform );
}

//========================================================================
const Vector3D& AffineTransform3D::GetXAxis( void ) const
{
	return linearTransform.GetXAxis();
}

//========================================================================
const Vector3D& AffineTransform3D::GetYAxis( void ) const
{
	return linearTransform.GetYAxis();
}

//========================================================================
const Vector3D& AffineTransform3D::GetZAxis( void ) const
{
	return linearTransform.GetZAxis();
}

//========================================================================
Vector3D& AffineTransform3D::GetXAxis( void )
{
	return linearTransform.GetXAxis();
}

//========================================================================
Vector3D& AffineTransform3D::GetYAxis( void )
{
	return linearTransform.GetYAxis();
}

//========================================================================
Vector3D& AffineTransform3D::GetZAxis( void )
{
	return linearTransform.GetZAxis();
}

//========================================================================
const Vector3D& AffineTransform3D::GetTranslation( void ) const
{
	return translationVector;
}

//========================================================================
Vector3D& AffineTransform3D::GetTranslation( void )
{
	return translationVector;
}

//========================================================================
const LinearTransform3D& AffineTransform3D::GetLinearTransform( void ) const
{
	return linearTransform;
}

//========================================================================
LinearTransform3D& AffineTransform3D::GetLinearTransform( void )
{
	return linearTransform;
}

//========================================================================
Scalar AffineTransform3D::Determinant( void ) const
{
	return linearTransform.Determinant();
}

//========================================================================
void AffineTransform3D::Transform( Vector3D& result, const Vector3D& vector ) const
{
	linearTransform.Transform( result, vector );
	result.Add( translationVector );
}

//========================================================================
void AffineTransform3D::Transform( Vector3D* results, const Vector3D* vectors, unsigned int count ) const
{
	for( unsigned int index = 0; index < count; index++ )
		Transform( results[ index ], vectors[ index ] );
}

//========================================================================
bool AffineTransform3D::Decompose( AffineTransform3D& scale, AffineTransform3D& shear, AffineTransform3D& rotation, AffineTransform3D& translation ) const
{
	scale.Identity();
	shear.Identity();
	rotation.Identity();
	translation.Identity();

	if( !linearTransform.Decompose( scale.linearTransform, shear.linearTransform, rotation.linearTransform ) )
		return false;

	translation.translationVector.Set( translationVector );
	return true;
}

//========================================================================
bool AffineTransform3D::Invert( AffineTransform3D& result ) const
{
	if( !linearTransform.Invert( result.linearTransform ) )
		return false;

	result.linearTransform.Transform( result.translationVector, translationVector );
	result.translationVector.Scale( -1.0 );
	return true;
}

//========================================================================
bool AffineTransform3D::Invert( void )
{
	AffineTransform3D result;
	if( !Invert( result ) )
		return false;

	Set( result );
	return true;
}

//========================================================================
void AffineTransform3D::OrthogonalInvert( AffineTransform3D& result ) const
{
	linearTransform.OrthogonalInvert( result.linearTransform );
	result.linearTransform.Transform( result.translationVector, translationVector );
	result.translationVector.Scale( -1.0 );
}

//========================================================================
void AffineTransform3D::OrthogonalInvert( void )
{
	AffineTransform3D result;
	OrthogonalInvert( result );
	Set( result );
}

//========================================================================
void AffineTransform3D::Concatinate( const AffineTransform3D& firstTransform, const AffineTransform3D& secondTransform )
{
	AffineTransform3D result;
	result.linearTransform.Concatinate( firstTransform.linearTransform, secondTransform.linearTransform );
	secondTransform.Transform( result.translationVector, firstTransform.translationVector );
	result.translationVector.Add( secondTransform.translationVector );
	Set( result );
}

// AffineTransform3D.cpp