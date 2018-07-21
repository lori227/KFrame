#ifndef __KF_BUFFER_H__
#define __KF_BUFFER_H__

#include "KFMap.h"
#include "KFInclude.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    class KFBuffer
    {
    public:
        KFBuffer()
        {
            _length = 0;
            _buffer = nullptr;
        }

        ~KFBuffer()
        {
            delete[] _buffer;
            _length = 0;
        }

    public:
        T* _buffer;
        uint32 _length;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    class KFThreadBuffer
    {
    public:
        KFThreadBuffer();
        ~KFThreadBuffer();

        // int8
        int8* GetInt8( uint32 length, const char* function, uint32 line );

        // uint8
        uint8* GetUInt8( uint32 length, const char* function, uint32 line );

    private:
        // 线程锁
        KFMutex* _kf_mutex;

        // buff列表
        typedef std::pair< uint32, uint32 > BuffKey;
        KFMap< BuffKey, const BuffKey&, KFBuffer< int8 > > _kf_int8;
        KFMap< BuffKey, const BuffKey&, KFBuffer< uint8 > > _kf_uint8;
    };
}

#endif
