#include "KFLibrary.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include "windows.h"
#else
    #include <dlfcn.h>
    #include <errno.h>
#endif

namespace KFrame
{
    KFLibrary::KFLibrary()
    {
        _instance = nullptr;
    }

    KFLibrary::~KFLibrary()
    {
        UnLoad();
    }

    bool KFLibrary::Load( const std::string& path, const std::string& name )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _path = path + name + ".dll";
        _instance = LoadLibraryEx( _path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
        if ( _instance == nullptr )
        {
            __LOG_ERROR__( "open result=[{}]", GetLastError() );
        }
#else
        _path = path + name + ".so";
        _instance = dlopen( _path.c_str(), RTLD_GLOBAL | RTLD_NOW );
        if ( _instance == nullptr )
        {
            __LOG_ERROR__( "open result=[{}]", dlerror() );
        }
#endif

        return _instance != nullptr;
    }

    void KFLibrary::UnLoad()
    {
        if ( _instance == nullptr )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        auto result = FreeLibrary( ( HMODULE )_instance );
        if ( result == TRUE )
        {
            __LOG_INFO__( "close [{}] ok", _path );
        }
        else
        {
            __LOG_ERROR__( "close [{}] failed=[{}]", _path, GetLastError() );
        }
#else
        auto result = dlclose( _instance );
        __LOG_INFO__( "close [{}] result=[{}:{}]", _path, result, dlerror() );
#endif

        _instance = nullptr;
    }

    void* KFLibrary::GetFunction( const char* function )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return GetProcAddress( ( HMODULE )_instance, function );
#else
        return dlsym( _instance, function );
#endif
    }
}
