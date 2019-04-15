#ifndef __KF_INT64_H__
#define __KF_INT64_H__

#include "KFData.h"

namespace KFrame
{
    class KFInt64 : public KFData
    {
    public:
        KFInt64();
        virtual ~KFInt64();

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
        virtual int64 GetInt64() {
            return _data;
        }
        virtual void SetInt64( int64 value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        int64 _data;
    };
}


#endif
