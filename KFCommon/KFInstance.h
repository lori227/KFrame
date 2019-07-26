#ifndef __KF_INSTANCE_H__
#define __KF_INSTANCE_H__

///////////////////////////////////////////////////////////////////
// 接口父类
////////////////////////////////////////////////////////////////////
namespace KFrame
{
    template< typename T >
    class KFInstance
    {
    public:
        virtual ~KFInstance() = default;

        static inline void Initialize( T* instance )
        {
            _instance = instance;
        }

        static inline T* Instance()
        {
            return _instance;
        }

    protected:
        KFInstance() = default;
    private:
        static T* _instance;
    };

    template< typename T >
    T* KFInstance< T >::_instance = nullptr;
}

#endif