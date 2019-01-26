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

    template< class T >
    struct KFUniqueResult
    {
        typedef std::unique_ptr< KFResult< T > > Type;
    };

#define __NEW_RESULT__( type ) \
    KFResult< type >::UniqueType  kfresult( new KFResult< type >() );
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFBaseResultQueue
    {
    public:
        virtual ~KFBaseResultQueue() = default;
        virtual void Free() = 0;
    };


    template< class T >
    class KFResultQueue : public KFBaseResultQueue
    {
    public:
        KFResultQueue() = default;
        ~KFResultQueue()
        {
            KFLocker locker( _kf_mutex );
            for ( auto kfresult : _idle_list )
            {
                __KF_DELETE__( KFResult< T >, kfresult );
            }
            _idle_list.clear();

            for ( auto kfresult : _use_list )
            {
                __KF_DELETE__( KFResult< T >, kfresult );
            }
            _use_list.clear();
        }

        // 申请
        KFResult< T >* Alloc()
        {
            KFResult< T >* kfresult = nullptr;

            KFLocker locker( _kf_mutex );
            if ( _idle_list.empty() )
            {
                kfresult = __KF_NEW__( KFResult< T > );
            }
            else
            {
                kfresult = _idle_list.front();
                _idle_list.pop_front();
            }

            kfresult->Reset();
            _use_list.push_back( kfresult );
            return kfresult;
        }

        // 释放
        void Free()
        {
            KFLocker locker( _kf_mutex );
            if ( !_use_list.empty() )
            {
                _idle_list.splice( _idle_list.end(), _use_list );
            }
        }

    private:
        // 队列锁
        KFMutex _kf_mutex;

        // 空闲的列表
        std::list < KFResult< T >* > _idle_list;

        // 使用中的列表
        std::list < KFResult< T >* > _use_list;
    };
}

#endif