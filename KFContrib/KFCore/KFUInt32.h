#ifndef __KF_UINT32_H__
#define __KF_UINT32_H__

#include "KFData.h"

namespace KFrame
{
    class KFUInt32 : public KFData
    {
    public:
        KFUInt32();
        virtual ~KFUInt32() = default;

        virtual void Reset( bool isdelete = true );

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
        virtual uint32 GetUInt32();
        virtual uint32 SetUInt32( uint32 value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 属性
        uint32 _data = 0u;
    };
}


#endif
