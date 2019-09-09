#ifndef __KF_RESULT_H__
#define __KF_RESULT_H__

#include "KFInclude.h"
#include "KFEnum.h"
#include "KFUtility/KFMutex.h"

namespace KFrame
{
    class KFBaseResult
    {
    public:
        KFBaseResult()
        {
            _result = KFEnum::Ok;
        }
        virtual ~KFBaseResult() = default;

        inline bool IsOk() const
        {
            return _result == KFEnum::Ok;
        }

        inline void Reset()
        {
            _result = KFEnum::Ok;
        }

        inline int32 GetResult() const
        {
            return _result;
        }

        inline void SetResult( int32 result )
        {
            _result = result;
        }

    protected:
        // 执行结果
        int32 _result;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    class KFResult : public KFBaseResult
    {
    public:
        typedef std::unique_ptr< KFResult< T > > UniqueType;
    public:

        KFResult()
        {
            _value = T();
        }

        inline void Reset()
        {
            _value = T();
            _result = KFEnum::Ok;
        }

    public:
        // 数值
        T _value;
    };

#define __NEW_RESULT__( type ) \
    KFResult< type >::UniqueType  kfresult( new KFResult< type >() );
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif