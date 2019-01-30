#ifndef __KF_UINT64_H__
#define __KF_UINT64_H__

#include "KFData.h"

namespace KFrame
{
    class KFUInt64 : public KFData
    {
    public:
        KFUInt64();
        virtual ~KFUInt64();

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
        virtual uint64 GetUInt64() {
            return _data;
        }
        virtual void SetUInt64( uint64 value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        uint64 _data;
    };
}


#endif
