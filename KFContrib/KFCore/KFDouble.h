#ifndef __KF_DOUBLE_H__
#define __KF_DOUBLE_H__

#include "KFData.h"

namespace KFrame
{
    class KFDouble : public KFData
    {
    public:
        KFDouble() = default;
        virtual ~KFDouble() = default;

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

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
        virtual double GetDouble();
        virtual double SetDouble( double value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        double _data = 0.0f;
    };
}


#endif
