#ifndef __KF_INT64_H__
#define __KF_INT64_H__

#include "KFData.h"

namespace KFrame
{
    class KFInt64 : public KFData
    {
    public:
        KFInt64() = default;
        virtual ~KFInt64() = default;

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
        virtual int64 GetInt64();
        virtual int64 SetInt64( int64 value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        int64 _data = 0;
    };
}


#endif
