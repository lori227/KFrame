// Vector2D.h

#pragma once

//========================================================================
class MATH_LIBRARY_API Math3D::Vector2D
{
public:

    Vector2D( void );
    Vector2D( const Vector2D& vector );
    Vector2D( Scalar x, Scalar y );
    ~Vector2D( void );

    Vector2D& Set( const Vector2D& vector );
    void Get( Vector2D& result ) const;

    Vector2D& Set( Scalar x, Scalar y );
    void Get( Scalar& x, Scalar& y ) const;

    void SetX( Scalar x );
    void SetY( Scalar y );

    Scalar GetX( void ) const;
    Scalar GetY( void ) const;

    Scalar Length( void ) const;

    bool SetNormalized( const Vector2D& vector );
    bool GetNormalized( Vector2D& result ) const;

    Scalar Angle( const Vector2D& vector ) const;

    Vector2D& operator=( const Vector2D& vector );
    Vector2D operator+( const Vector2D& vector ) const;
    Vector2D operator-( const Vector2D& vector ) const;
    Vector2D operator*( Scalar scalar ) const;

    void operator+=( const Vector2D& vector );
    void operator-=( const Vector2D& vector );

    Vector2D& Add( const Vector2D& vectorA, const Vector2D& vectorB );
    Vector2D& Add( const Vector2D& vectorB );
    Vector2D& Sub( const Vector2D& vectorA, const Vector2D& vectorB );
    Vector2D& Sub( const Vector2D& vectorB );
    Scalar Dot( const Vector2D& vector ) const;
    Vector2D& Scale( Scalar scalar );
    Vector2D& Scale( const Vector2D& vector, Scalar scalar );
    Vector2D& OrthoProject( const Vector2D& unitProjectionVector, const Vector2D& vector );
    Vector2D& OrthoReject( const Vector2D& unitRejectionVector, const Vector2D& vector );
    Vector2D& Rotate( Scalar angle );
    Vector2D& Rotate( const Vector2D& vector, Scalar angle );

    // Add two vectors together.
    static void Add( Vector2D& result, const Vector2D& vectorA, const Vector2D& vectorB );

    // Subtract one vector from another.
    static void Sub( Vector2D& result, const Vector2D& vectorA, const Vector2D& vectorB );

    // Take the dot product of two vectors.
    static Scalar Dot( const Vector2D& vectorA, const Vector2D& vectorB );

    // Scale the given vector by the given scalar.
    static void Scale( Vector2D& result, const Vector2D& vector, Scalar scalar );

    // Find the orthogonal projection of one vector onto another.  The given projection vector must be of unit length.
    static void OrthoProject( Vector2D& result, const Vector2D& unitProjectionVector, const Vector2D& vector );

    // Find the orthogonal rejection of one vector from another.  The given rejection vector must be of unit length.
    static void OrthoReject( Vector2D& result, const Vector2D& unitRejectionVector, const Vector2D& vector );

    // Rotate the given vector in the XY-plane about the given angle in radians.
    static void Rotate( Vector2D& result, const Vector2D& vector, Scalar angle );

    static const Vector2D xAxis;
    static const Vector2D yAxis;

private:

    Scalar x, y;
};

// Vector2D.h