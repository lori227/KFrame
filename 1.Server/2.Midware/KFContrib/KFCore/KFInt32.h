#ifndef __KF_INT32_H__
#define __KF_INT32_H__

#include "KFData.h"

namespace KFrame
{
    class KFInt32 : public KFData
    {
    public:
        KFInt32();
        virtual ~KFInt32();

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();
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
        virtual int32 GetInt32() {
            return _data;
        }
        virtual void SetInt32( int32 value ) {
            _data = value;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        // 属性
        int32 _data;
    };


}


#endif
