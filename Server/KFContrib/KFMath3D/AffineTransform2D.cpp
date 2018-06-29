// AffineTransform2D.cpp

#include "Math3D.h"

using namespace Math3D;

//========================================================================
AffineTransform2D::AffineTransform2D( void )
{
    Identity();
}

//========================================================================
AffineTransform2D::AffineTransform2D( const AffineTransform2D& transform )
{
    Set( transform );
}

//========================================================================
AffineTransform2D::AffineTransform2D( const LinearTransform2D& linearTransform, const Vector2D& translation )
{
    SetLinearTransform( linearTransform );
    SetTranslation( translation );
}

//========================================================================
AffineTransform2D::AffineTransform2D( const Vector2D& xAxis, const Vector2D& yAxis, const Vector2D& translation )
{
    linearTransform.SetXAxis( xAxis );
    linearTransform.SetYAxis( yAxis );
    translationVector.Set( translation );
}

//========================================================================
AffineTransform2D::~AffineTransform2D( void )
{
}

//========================================================================
void AffineTransform2D::Identity( void )
{
    linearTransform.Identity();
    translationVector.Set( 0.0, 0.0 );
}

//========================================================================
void AffineTransform2D::Set( const AffineTransform2D& transform )
{
    linearTransform.Set( transform.GetLinearTransform() );
    translationVector.Set( transform.GetTranslation() );
}

//========================================================================
void AffineTransform2D::Get( AffineTransform2D& transform ) const
{
    linearTransform.Get( transform.GetLinearTransform() );
    translationVector.Get( transform.translationVector );
}

//========================================================================
void AffineTransform2D::Set( const Vector2D& xAxis, const Vector2D& yAxis, const Vector2D& translation )
{
    linearTransform.SetXAxis( xAxis );
    linearTransform.SetYAxis( yAxis );
    translationVector.Set( translation );
}

//========================================================================
void AffineTransform2D::Get( Vector2D& xAxis, Vector2D& yAxis, Vector2D& translation ) const
{
    xAxis.Set( linearTransform.GetXAxis() );
    yAxis.Set( linearTransform.GetYAxis() );
    translation.Set( translationVector );
}

//========================================================================
void AffineTransform2D::SetXAxis( const Vector2D& xAxis )
{
    linearTransform.SetXAxis( xAxis );
}

//========================================================================
void AffineTransform2D::SetYAxis( const Vector2D& yAxis )
{
    linearTransform.SetYAxis( yAxis );
}

//========================================================================
void AffineTransform2D::SetTranslation( const Vector2D& translation )
{
    translationVector.Set( translation );
}

//========================================================================
void AffineTransform2D::SetTranslation( Scalar x, Scalar y )
{
    translationVector.Set( x, y );
}

//========================================================================
void AffineTransform2D::SetLinearTransform( const LinearTransform2D& linearTransform )
{
    this->linearTransform.Set( linearTransform );
}

//========================================================================
const Vector2D& AffineTransform2D::GetXAxis( void ) const
{
    return linearTransform.GetXAxis();
}

//========================================================================
const Vector2D& AffineTransform2D::GetYAxis( void ) const
{
    return linearTransform.GetYAxis();
}

//========================================================================
Vector2D& AffineTransform2D::GetXAxis( void )
{
    return linearTransform.GetXAxis();
}

//========================================================================
Vector2D& AffineTransform2D::GetYAxis( void )
{
    return linearTransform.GetYAxis();
}

//========================================================================
const Vector2D& AffineTransform2D::GetTranslation( void ) const
{
    return translationVector;
}

//========================================================================
Vector2D& AffineTransform2D::GetTranslation( void )
{
    return translationVector;
}

//========================================================================
const LinearTransform2D& AffineTransform2D::GetLinearTransform( void ) const
{
    return linearTransform;
}

//========================================================================
LinearTransform2D& AffineTransform2D::GetLinearTransform( void )
{
    return linearTransform;
}

//========================================================================
Scalar AffineTransform2D::Determinant( void ) const
{
    Scalar det = linearTransform.Determinant();
    return det;
}

//========================================================================
void AffineTransform2D::Transform( Vector2D& result, const Vector2D& vector ) const
{
    linearTransform.Transform( result, vector );
    result.Add( translationVector );
}

//========================================================================
void AffineTransform2D::Transform( Vector2D* results, const Vector2D* vectors, unsigned int count ) const
{
    for ( unsigned int index = 0; index < count; index++ )
        Transform( results[ index ], vectors[ index ] );
}

//========================================================================
bool AffineTransform2D::Decompose( AffineTransform2D& scale, AffineTransform2D& shear, AffineTransform2D& rotation, AffineTransform2D& translation ) const
{
    scale.translationVector.Set( 0.0, 0.0 );
    shear.translationVector.Set( 0.0, 0.0 );
    rotation.translationVector.Set( 0.0, 0.0 );
    if ( !linearTransform.Decompose( scale.linearTransform, shear.linearTransform, rotation.linearTransform ) )
        return false;

    translation.linearTransform.Identity();
    translation.SetTranslation( translationVector );
    return true;
}

//========================================================================
bool AffineTransform2D::Invert( AffineTransform2D& result ) const
{
    if ( !linearTransform.Invert( result.linearTransform ) )
        return false;

    result.linearTransform.Transform( result.translationVector, translationVector );
    result.translationVector.Scale( -1.0 );
    return true;
}

//========================================================================
bool AffineTransform2D::Invert( void )
{
    AffineTransform2D result;
    if ( !Invert( result ) )
        return false;

    Set( result );
    return true;
}

//========================================================================
void AffineTransform2D::OrthogonalInvert( AffineTransform2D& result ) const
{
    linearTransform.OrthogonalInvert( result.linearTransform );
    result.linearTransform.Transform( result.translationVector, translationVector );
    result.translationVector.Scale( -1.0 );
}

//========================================================================
void AffineTransform2D::OrthogonalInvert( void )
{
    AffineTransform2D result;
    OrthogonalInvert( result );
    Set( result );
}

//========================================================================
void AffineTransform2D::Concatinate( const AffineTransform2D& firstTransform, const AffineTransform2D& secondTransform )
{
    AffineTransform2D result;
    result.linearTransform.Concatinate( firstTransform.linearTransform, secondTransform.linearTransform );
    secondTransform.Transform( result.translationVector, firstTransform.translationVector );
    result.translationVector.Add( secondTransform.translationVector );
    Set( result );
}

// AffineTransform2D.cpp