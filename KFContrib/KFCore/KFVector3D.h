#ifndef __KF_VECTOR3D_H__
#define __KF_VECTOR3D_H__

#include "KFData.h"
#include "KFMath3D/Math3D.h"

namespace KFrame
{
    class KFVector3D : public KFData
    {
    public:
        KFVector3D();
        virtual ~KFVector3D() = default;

        virtual void Reset( bool isdelete = true );

        // 是否有效
        virtual bool IsValid();

        // 保存 赋值
        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

    protected:
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const Math3D::Vector3D& GetVector3D();
        virtual const Math3D::Vector3D& SetVector3D( Math3D::Vector3D& value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        Math3D::Vector3D _data;
    };
}


#endif
