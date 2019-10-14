#include "KFVector3D.h"

namespace KFrame
{
    const Math3D::Vector3D& KFVector3D::GetVector3D()
    {
        return _data;
    }

    const Math3D::Vector3D& KFVector3D::SetVector3D( Math3D::Vector3D& value )
    {
        _data = value;
        return _data;
    }

    void KFVector3D::Reset()
    {
        _data.Set( 0.0f, 0.0f, 0.0f );
    }

    bool KFVector3D::IsValid()
    {
        return ( _data.GetX() != 0.0f || _data.GetY() != 0.0f || _data.GetZ() != 0.0f );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFVector3D::CopyFrom( KFData* kfother )
    {
        _data = kfother->Get<Math3D::Vector3D>();
    }

    void KFVector3D::SaveTo( KFData* kfother )
    {
        kfother->Set( _data );
    }

    std::string KFVector3D::ToString()
    {
        return __FORMAT__( "(:.2f,:.2f,:.2f)", _data.GetX(), _data.GetY(), _data.GetY() );
    }

    void KFVector3D::FromString( const std::string& value )
    {
        auto temp = value;
        KFUtility::SplitString( temp, "(" );

        auto x = KFUtility::SplitValue< double >( temp, "," );
        auto y = KFUtility::SplitValue< double >( temp, "," );
        auto z = KFUtility::SplitValue< double >( temp, "," );

        _data.Set( x, y, z );
    }
}