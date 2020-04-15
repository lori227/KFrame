#ifndef __KF_INT32_H__
#define __KF_INT32_H__

#include "KFData.h"

namespace KFrame
{
    class KFInt32 : public KFData
    {
    public:
        KFInt32();
        virtual ~KFInt32() = default;

        virtual void Reset( bool isdelete = true );

        // 是否达到了最大值
        virtual bool IsFull();

        // 是否有效
        virtual bool IsValid();

        // 初始化数值
        virtual void InitData();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 保存 赋值
        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );
    protected:
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int32 GetInt32();
        virtual int32 SetInt32( int32 value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 属性
        int32 _data = 0;
    };


}


#endif
