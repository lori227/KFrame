#ifndef __KF_UINT64_H__
#define __KF_UINT64_H__

#include "KFData.h"

namespace KFrame
{
    class KFUInt64 : public KFData
    {
    public:
        KFUInt64();
        virtual ~KFUInt64() = default;

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 是否达到了最大值
        virtual bool IsFull();

        // 初始化数值
        virtual void InitData();

        // 保存 赋值
        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

    protected:
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual uint64 GetUInt64();
        virtual uint64 SetUInt64( uint64 value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        uint64 _data = 0u;
    };
}


#endif
