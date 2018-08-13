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
        _instance = nullptr;
    }

    bool KFLibrary::Load( const std::string& path, const std::string& name )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _path = path + name + ".dll";
        _instance = LoadLibraryEx( _path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
        return _instance != nullptr;
#else
        _path = path + name + ".so";
        _instance = dlopen( _path.c_str(), RTLD_GLOBAL | RTLD_LAZY );
        if ( _instance == nullptr )
        {
            __LOG_LOCAL__( KFLogEnum::Init, "dlopen - {}", dlerror() );
        }
        return _instance != nullptr;
#endif
    }

    void KFLibrary::UnLoad()
    {
        if ( _instance == nullptr )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        FreeLibrary( ( HMODULE )_instance );
#else
        dlclose( _instance );
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
