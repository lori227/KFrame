// AffineTransform3D.h

#pragma once

//========================================================================
class MATH_LIBRARY_API Math3D::AffineTransform3D
{
public:

    AffineTransform3D( void );
    AffineTransform3D( const AffineTransform3D& transform );
    AffineTransform3D( const LinearTransform3D& linearTransform, const Vector3D& translation );
    AffineTransform3D( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis, const Vector3D& translation );
    ~AffineTransform3D( void );

    // Make this the identity function.
    void Identity( void );

    void Set( const AffineTransform3D& transform );
    void Get( AffineTransform3D& transform ) const;

    void Set( const LinearTransform3D& linearTransform, const Vector3D& translation );
    void Get( LinearTransform3D& linearTransform, Vector3D& translation ) const;

    void Set( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis, const Vector3D& translation );
    void Get( Vector3D& xAxis, Vector3D& yAxis, Vector3D& zAxis, Vector3D& translation ) const;

    void SetXAxis( const Vector3D& xAxis );
    void SetYAxis( const Vector3D& yAxis );
    void SetZAxis( const Vector3D& zAxis );

    void SetTranslation( const Vector3D& translation );
    void SetTranslation( Scalar x, Scalar y, Scalar z );

    void SetLinearTransform( const LinearTransform3D& linearTransform );

    const Vector3D& GetXAxis( void ) const;
    const Vector3D& GetYAxis( void ) const;
    const Vector3D& GetZAxis( void ) const;

    Vector3D& GetXAxis( void );
    Vector3D& GetYAxis( void );
    Vector3D& GetZAxis( void );

    const Vector3D& GetTranslation( void ) const;
    Vector3D& GetTranslation( void );

    const LinearTransform3D& GetLinearTransform( void ) const;
    LinearTransform3D& GetLinearTransform( void );

    Scalar Determinant( void ) const;

    void Transform( Vector3D& result, const Vector3D& vector ) const;
    void Transform( Vector3D* results, const Vector3D* vectors, unsigned int count ) const;

    // Decompose this transformation into the product of a scale, shear, rotation and translation transformation, in that order.
    // This type of decomposition, which is based upon the Gram-Schmidt process, exists if and only if the transformation is invertable.
    bool Decompose( AffineTransform3D& scale, AffineTransform3D& shear, AffineTransform3D& rotation, AffineTransform3D& translation ) const;

    // Find the inverse transformation.
    bool Invert( AffineTransform3D& result ) const;
    bool Invert( void );

    // Assuming that the linear transformation part is orthogonal, find the inverse transformation.
    // If you have a rigidy-body transformation, this is a faster way of inverting than using Invert().
    void OrthogonalInvert( AffineTransform3D& result ) const;
    void OrthogonalInvert( void );

    // This is function composition.  Make this transform be what performs the first given transform, then the second.
    void Concatinate( const AffineTransform3D& firstTransform, const AffineTransform3D& secondTransform );

private:

    LinearTransform3D linearTransform;
    Vector3D translationVector;
};

// AffineTransform3D.h