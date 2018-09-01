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
    template< class T >
    class KFResultQueue
    {
    public:
        KFResultQueue() = default;
        ~KFResultQueue()
        {
            for ( auto kfresult : _idle_list )
            {
                __KF_DESTROY__( KFResult<T>, kfresult );
            }
            _idle_list.clear();

            for ( auto kfresult : _use_list )
            {
                __KF_DESTROY__( KFResult<T>, kfresult );
            }
            _use_list.clear();
        }

        // 申请
        KFResult< T >* Alloc()
        {
            KFResult< T >* kfresult = nullptr;
            if ( _idle_list.empty() )
            {
                kfresult = __KF_CREATE_BATCH__( KFResult< T >, 10 );
            }
            else
            {
                kfresult = _idle_list.front();
                _idle_list.pop_front();
            }

            _use_list.push_back( kfresult );
            return kfresult;
        }

        // 释放
        void

    private:
        // 空闲的列表
        std::list < KFResult< T >* > _idle_list;

        // 使用中的列表
        std::list < KFResult< T >* > _use_list;
    };
}

#endif