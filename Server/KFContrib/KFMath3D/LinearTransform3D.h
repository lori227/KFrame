// LinearTransform3D.h

#pragma once

//========================================================================
// These are linear transformations (linear functions) determined
// entirely be three vectors.  Such functions are invertible if and only
// if these three vectors form a basis for a 3-dimensional vector space.
// The concept of such transformations is being represented here
// without the notion of a matrix or matrix algebra.  Think of an
// invertible instance of this class as a linear function from a
// 3-dimensional vector space onto itself.
class MATH_LIBRARY_API Math3D::LinearTransform3D
{
public:

    LinearTransform3D( void );
    LinearTransform3D( const LinearTransform3D& transform );
    LinearTransform3D( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis );
    ~LinearTransform3D( void );

    // Make this the identity function.
    void Identity( void );

    void Set( const LinearTransform3D& transform );
    void Get( LinearTransform3D& transform ) const;

    void Set( const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis );
    void Get( Vector3D& xAxis, Vector3D& yAxis, Vector3D& zAxis ) const;

    Vector3D& SetXAxis( const Vector3D& xAxis );
    Vector3D& SetYAxis( const Vector3D& yAxis );
    Vector3D& SetZAxis( const Vector3D& zAxis );

    const Vector3D& GetXAxis( void ) const;
    const Vector3D& GetYAxis( void ) const;
    const Vector3D& GetZAxis( void ) const;

    Vector3D& GetXAxis( void );
    Vector3D& GetYAxis( void );
    Vector3D& GetZAxis( void );

    Scalar Determinant( void ) const;

    // Reinterpret the given vector in terms of our basis.  (Evaluate the linear function that is this transformation.)
    void Transform( Vector3D& result, const Vector3D& vector ) const;
    void Transform( Vector3D* results, const Vector3D* vectors, unsigned int count ) const;

    // Compose this linear transformation as a rotation, scale or shear transformation.
    void ComposeRotation( const Vector3D& unitAxis, Scalar angle );
    void ComposeScale( Scalar xScale, Scalar yScale, Scalar zScale );
    void ComposeShear( Scalar xyShear, Scalar yzShear, Scalar xzShear );

    // Decompose this transformation into the product of a scale, shear and rotation transformation, in that order.
    // This type of decomposition, which is based upon the Gram-Schmidt orthogonalization process, exists if and
    // only if the transformation is invertable.
    bool Decompose( LinearTransform3D& scale, LinearTransform3D& shear, LinearTransform3D& rotation ) const;

    // Find the inverse transformation.
    bool Invert( LinearTransform3D& result ) const;
    bool Invert( void );

    // Assuming that this transformation is orthogonal, find the inverse transformation.  This is equivilent to taking a transpose in matrix land.
    void OrthogonalInvert( LinearTransform3D& result ) const;
    void OrthogonalInvert( void );

    // This is function composition.  Make this transform be what performs the first given transform, then the second.
    void Concatinate( const LinearTransform3D& firstTransform, const LinearTransform3D& secondTransform );

    // Let this linear transform be the one that takes preVectorA to postVectorA, preVectorB to postVectorB and
    // preVectorC to postVectorC.  If there is no such transform, or there is no unique solution, then false is returned.
    bool Solve( const Vector3D& preVectorA, const Vector3D& preVectorB, const Vector3D& preVectorC, const Vector3D& postVectorA, const Vector3D& postVectorB, const Vector3D& postVectorC );

    // If this transform is a rotation, this can be used to avoid round-off error.
    bool Orthonormalize( void );

private:

    Vector3D xAxis, yAxis, zAxis;
};

// LinearTransform3D.h