// Vector3D.h

#pragma once

//========================================================================
// This is a vector taken from a 3-dimensional vector space.
class MATH_LIBRARY_API Math3D::Vector3D
{
public:

    Vector3D( void );
    Vector3D( const Vector3D& vector );
    Vector3D( Scalar x, Scalar y, Scalar z );
    ~Vector3D( void );

    Vector3D& Set( const Vector3D& vector );
    void Get( Vector3D& result ) const;

    Vector3D& Set( Scalar x, Scalar y, Scalar z );
    void Get( Scalar& x, Scalar& y, Scalar& z ) const;

    void SetX( Scalar x );
    void SetY( Scalar y );
    void SetZ( Scalar z );

    Scalar GetX( void ) const;
    Scalar GetY( void ) const;
    Scalar GetZ( void ) const;

    Scalar Length( void ) const;

    bool SetNormalized( const Vector3D& vector );
    bool GetNormalized( Vector3D& result ) const;

    Scalar Angle( const Vector3D& vector ) const;

    Vector3D& operator=( const Vector3D& vector );
    Vector3D operator+( const Vector3D& vector ) const;
    Vector3D operator-( const Vector3D& vector ) const;
    Vector3D operator*( const Vector3D& vector ) const;
    Vector3D operator*( Scalar scalar ) const;

    void operator+=( const Vector3D& vector );
    void operator-=( const Vector3D& vector );

    Vector3D& Add( const Vector3D& vectorA, const Vector3D& vectorB );
    Vector3D& Add( const Vector3D& vectorB );
    Vector3D& Sub( const Vector3D& vectorA, const Vector3D& vectorB );
    Vector3D& Sub( const Vector3D& vectorB );
    Vector3D& Cross( const Vector3D& vectorA, const Vector3D& vectorB );
    Scalar Dot( const Vector3D& vector ) const;
    Vector3D& Scale( const Vector3D& vector, Scalar scalar );
    Vector3D& Scale( Scalar scalar );
    Vector3D& OrthoProject( const Vector3D& unitProjectionVector, const Vector3D& vector );
    Vector3D& OrthoReject( const Vector3D& unitRejectionVector, const Vector3D& vector );
    Vector3D& Rotate( const Vector3D& unitAxis, Scalar angle );
    Vector3D& Rotate( const Vector3D& vector, const Vector3D& unitAxis, Scalar angle );

    // Add two vectors together.
    static void Add( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB );

    // Subtract one vector from another.
    static void Sub( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB );

    // Take the cross product of two vectors.  This product is non-commutative.  (In fact, it's anti-commutative.)
    static void Cross( Vector3D& result, const Vector3D& vectorA, const Vector3D& vectorB );

    // Take the dot product of two vectors.  This product is commutative.
    static Scalar Dot( const Vector3D& vectorA, const Vector3D& vectorB );

    // Scale the given vector by the given scalar.  This is the scalar-vector product.
    static void Scale( Vector3D& result, const Vector3D& vector, Scalar scalar );

    // Find the orthogonal projection of one vector onto another.  The given projection vector must be of unit length.
    static void OrthoProject( Vector3D& result, const Vector3D& unitProjectionVector, const Vector3D& vector );

    // Find the orthogonal rejection of one vector from another.  The given rejection vector must be of unit length.
    static void OrthoReject( Vector3D& result, const Vector3D& unitRejectionVector, const Vector3D& vector );

    // Rotate the given vector about the given unit-length axis by the given angle in radians.
    static void Rotate( Vector3D& result, const Vector3D& vector, const Vector3D& unitAxis, Scalar angle );

    // Find the rotation that rotates one of the given vectors into the other.
    static void Rotation( Vector3D& unitAxis, Scalar& angle, const Vector3D& vectorA, const Vector3D& vectorB );

    static const Vector3D xAxis;
    static const Vector3D yAxis;
    static const Vector3D zAxis;

private:

    Scalar x, y, z;
};

// Vector3D.h