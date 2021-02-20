#ifndef __KF_RESULT_H__
#define __KF_RESULT_H__

#include "KFEnum.h"
#include "KFDefine.h"

namespace KFrame
{
    template<class T>
    class KFResult
    {
    public:
        typedef std::unique_ptr< KFResult<T> > UniqueType;

        // 构造函数
        KFResult()
        {
            _value = T();
            _code = KFEnum::Ok;
        }

        inline bool IsOk() const
        {
            return _code == KFEnum::Ok;
        }

        inline void Reset()
        {
            _value = T();
            _code = KFEnum::Ok;
        }

        inline int32 GetCode() const
        {
            return _code;
        }

        inline void SetCode( int32 code )
        {
            _code = code;
        }

    protected:
        // 执行结果
        int32 _code;

    public:
        // 数值
        T _value;
    };

#define __NEW_RESULT__( type ) \
    KFResult< type >::UniqueType result( new KFResult< type >() );
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif