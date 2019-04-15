#ifndef __KF_SINGLETION_H__
#define __KF_SINGLETION_H__

/************************************************************************

					单例模式


************************************************************************/
// 多线程安全
// #define barrier() __asm__ volatile ("lwsync")
// 保证返回之前是构造完整的对象

namespace KFrame
{
    template< typename T >
    class KFSingleton
    {
    public:
        virtual ~KFSingleton() = default;

        template <class...P>
        static inline T* Instance( P&& ... params )
        {
            if ( _instance == nullptr )
            {
                _instance = new T( std::forward<P>( params )... );
                std::atexit( &KFSingleton::UnInstance );
            }

            return _instance;
        }


    protected:
        KFSingleton()
        {
            _instance = nullptr;
        }

        static inline void UnInstance()
        {
            if ( _instance != nullptr )
            {
                delete _instance;
            }
        }

    private:
        KFSingleton( const KFSingleton& ) = delete;
        KFSingleton& operator = ( const KFSingleton& ) = delete;

    private:
        static T* _instance;
    };

    template< typename T >
    T* KFSingleton< T >::_instance = nullptr;
}

#endif