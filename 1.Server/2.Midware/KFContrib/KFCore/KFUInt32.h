#ifndef __KF_UINT32_H__
#define __KF_UINT32_H__

#include "KFData.h"

namespace KFrame
{
    class KFUInt32 : public KFData
    {
    public:
        KFUInt32();
        virtual ~KFUInt32();

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
        virtual uint32 GetUInt32() {
            return _data;
        }
        virtual void SetUInt32( uint32 value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        uint32 _data;
    };
}


#endif
