#ifndef __KF_STRING_H__
#define __KF_STRING_H__

#include "KFData.h"

namespace KFrame
{
    class KFString : public KFData
    {
    public:
        KFString();
        virtual ~KFString() = default;

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
        virtual const std::string& GetString();
        virtual const std::string& SetString( const std::string& value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        std::string _data;
    };
}


#endif
