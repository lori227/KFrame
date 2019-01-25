// AffineTransform2D.h

#pragma once

//========================================================================
class MATH_LIBRARY_API Math3D::AffineTransform2D
{
public:

    AffineTransform2D( void );
    AffineTransform2D( const AffineTransform2D& transform );
    AffineTransform2D( const LinearTransform2D& linearTransform, const Vector2D& translation );
    AffineTransform2D( const Vector2D& xAxis, const Vector2D& yAxis, const Vector2D& translation );
    ~AffineTransform2D( void );

    // Make this the identity function.
    void Identity( void );

    void Set( const AffineTransform2D& transform );
    void Get( AffineTransform2D& transform ) const;

    void Set( const Vector2D& xAxis, const Vector2D& yAxis, const Vector2D& translation );
    void Get( Vector2D& xAxis, Vector2D& yAxis, Vector2D& translation ) const;

    void SetXAxis( const Vector2D& xAxis );
    void SetYAxis( const Vector2D& yAxis );

    void SetTranslation( const Vector2D& translation );
    void SetTranslation( Scalar x, Scalar y );

    void SetLinearTransform( const LinearTransform2D& linearTransform );

    const Vector2D& GetXAxis( void ) const;
    const Vector2D& GetYAxis( void ) const;
    Vector2D& GetXAxis( void );
    Vector2D& GetYAxis( void );

    const Vector2D& GetTranslation( void ) const;
    Vector2D& GetTranslation( void );

    const LinearTransform2D& GetLinearTransform( void ) const;
    LinearTransform2D& GetLinearTransform( void );

    Scalar Determinant( void ) const;

    void Transform( Vector2D& result, const Vector2D& vector ) const;
    void Transform( Vector2D* results, const Vector2D* vectors, unsigned int count ) const;

    // Decompose this transformation into the product of a scale, shear, rotation and translation transformation, in that order.
    // This type of decomposition, which is based upon the Gram-Schmidt process, exists if and only if the transformation is invertable.
    bool Decompose( AffineTransform2D& scale, AffineTransform2D& shear, AffineTransform2D& rotation, AffineTransform2D& translation ) const;

    // Find the inverse transformation.
    bool Invert( AffineTransform2D& result ) const;
    bool Invert( void );

    // Assuming that the linear transformation part is orthogonal, find the inverse transformation.
    void OrthogonalInvert( AffineTransform2D& result ) const;
    void OrthogonalInvert( void );

    // This is function composition.  Make this transform be what performs the first given transform, then the second.
    void Concatinate( const AffineTransform2D& firstTransform, const AffineTransform2D& secondTransform );

private:

    LinearTransform2D linearTransform;
    Vector2D translationVector;
};

// AffineTransform2D.h