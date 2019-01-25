// LinearTransform2D.h

#pragma once

//========================================================================
// These are linear transformations (linear functions) determined
// entirely be two vectors.  Such functions are invertible if and only
// if these two vectors form a basis for a 2-dimensional vector space.
// The concept of such transformations is being represented here
// without the notion of a matrix or matrix algebra.  Think of an
// invertible instance of this class as a linear function from a
// 2-dimensional vector space onto itself.
class MATH_LIBRARY_API Math3D::LinearTransform2D
{
public:

    LinearTransform2D( void );
    LinearTransform2D( const LinearTransform2D& transform );
    LinearTransform2D( const Vector2D& xAxis, const Vector2D& yAxis );
    ~LinearTransform2D( void );

    // Make this the identity function.
    void Identity( void );

    void Set( const LinearTransform2D& transform );
    void Get( LinearTransform2D& transform ) const;

    void Set( const Vector2D& xAxis, const Vector2D& yAxis );
    void Get( Vector2D& xAxis, Vector2D& yAxis ) const;

    Vector2D& SetXAxis( const Vector2D& xAxis );
    Vector2D& SetYAxis( const Vector2D& yAxis );

    const Vector2D& GetXAxis( void ) const;
    const Vector2D& GetYAxis( void ) const;

    Vector2D& GetXAxis( void );
    Vector2D& GetYAxis( void );

    Scalar Determinant( void ) const;

    // Reinterpret the given vector in terms of our basis.  (Evaluate the function that is this transformation.)
    void Transform( Vector2D& result, const Vector2D& vector ) const;
    void Transform( Vector2D* results, const Vector2D* vectors, unsigned int count ) const;

    // Compose this linear transformation as a rotation, scale or shear transformation.
    void ComposeRotation( Scalar angle );
    void ComposeScale( Scalar xScale, Scalar yScale );
    void ComposeShear( Scalar xyShear );

    // Decompose this transformation into the product of a scale, shear and rotation transformation, in that order.
    // This type of decomposition, which is based upon the Gram-Schmidt process, exists if and only if the transformation is invertable.
    bool Decompose( LinearTransform2D& scale, LinearTransform2D& shear, LinearTransform2D& rotation ) const;

    // Find the inverse transformation.
    bool Invert( LinearTransform2D& result ) const;
    bool Invert( void );

    // Assuming that this transformation is orthogonal, find the inverse transformation.  This is equivilent to taking a transpose in matrix land.
    void OrthogonalInvert( LinearTransform2D& result ) const;
    void OrthogonalInvert( void );

    // This is function composition.  Make this transform be what performs the first given transform, then the second.
    void Concatinate( const LinearTransform2D& firstTransform, const LinearTransform2D& secondTransform );

    // Let this linear transform be the one that takes preVectorA to postVectorA and preVectorB to postVectorB.
    // If there is no such transform, or there is no unique solution, then false is returned.
    bool Solve( const Vector2D& preVectorA, const Vector2D& preVectorB, const Vector2D& postVectorA, const Vector2D& postVectorB );

    // If this transform is a rotation, this can be used to avoid round-off error.
    bool Orthonormalize( void );

private:

    Vector2D xAxis, yAxis;
};

// LinearTransform2D.h