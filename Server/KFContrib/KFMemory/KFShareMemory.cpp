#include "KFShareMemory.h"
#include "Poco/SharedMemory.h"

namespace KFrame
{
    KFShareMemory::KFShareMemory()
    {
        _share_memory = nullptr;
    }

    KFShareMemory::~KFShareMemory()
    {
        ReleaseMemory();
    }

    char* KFShareMemory::CreateMemory( const std::string& name, uint32 length )
    {
        if ( _share_memory == nullptr )
        {
            _share_memory = __KF_NEW__( Poco::SharedMemory, name, length, Poco::SharedMemory::AM_WRITE );
        }

        auto sharememory = reinterpret_cast< Poco::SharedMemory* >( _share_memory );
        return sharememory->begin();
    }

    void KFShareMemory::ReleaseMemory()
    {
        auto sharememoey = reinterpret_cast< Poco::SharedMemory* >( _share_memory );
        __KF_DELETE__( sharememoey );
    }
}