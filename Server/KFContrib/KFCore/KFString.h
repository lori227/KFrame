#ifndef __KF_STRING_H__
#define __KF_STRING_H__

#include "KFData.h"

namespace KFrame
{
    class KFString : public KFData
    {
    public:
        KFString();
        virtual ~KFString();

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
        virtual const std::string& GetString() {
            return _data;
        }
        virtual void SetString( const std::string& value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        std::string _data;
    };
}


#endif
