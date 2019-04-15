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
        virtual ~KFVector3D();

        virtual void Reset();

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
        virtual const Math3D::Vector3D& GetVector3D() {
            return _data;
        }
        virtual void SetVector3D( Math3D::Vector3D& value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        Math3D::Vector3D _data;
    };
}


#endif
