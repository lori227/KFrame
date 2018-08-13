#ifndef __KF_RESULT_H__
#define __KF_RESULT_H__

#include "KFInclude.h"
#include "KFEnum.h"

namespace KFrame
{
    class KFBaseResult
    {
    public:
        KFBaseResult()
        {
            _result = 0;
        }

        inline bool IsOk() const
        {
            return _result == KFEnum::Ok;
        }

        inline void Reset()
        {
            _result = 0;
        }

        inline int32 GetResult() const
        {
            return _result;
        }

        inline void SetResult( int32 result )
        {
            _result = result;
        }

    private:
        // 执行结果
        int32 _result;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    class KFResult : public KFBaseResult
    {
    public:
        KFResult()
        {
            _value = T();
        }

    public:
        // 数值
        T _value;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif