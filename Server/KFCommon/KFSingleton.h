#ifndef __KF_SINGLETION_H__
#define __KF_SINGLETION_H__

/************************************************************************

					单例模式


************************************************************************/
namespace KFrame
{
    template< typename T >
    class KFSingleton
    {
    public:
        virtual ~KFSingleton()
        {
            if ( _instance != nullptr )
            {
                delete _instance;
                _instance = nullptr;
            }
        }

        template <class...P>
        static T* Instance( P&& ... params )
        {
            if ( _instance == nullptr )
            {
                _instance = new T( std::forward<P>( params )... );
            }

            return _instance;
        }

    protected:
        KFSingleton()
        {
            _instance = nullptr;
        }

    private:
        KFSingleton( const KFSingleton& );
        KFSingleton& operator = ( const KFSingleton& );

    private:
        static T* _instance;
    };

    template< typename T >
    T* KFSingleton< T >::_instance = nullptr;
}

#endif