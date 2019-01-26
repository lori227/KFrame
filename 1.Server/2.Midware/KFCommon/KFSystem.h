#ifndef __KF_SYSTEM_H__
#define __KF_SYSTEM_H__

#ifdef _DEBUG
    #define __KF_DEBUG__
#else
    #define __KF_RELEASE__
#endif // _DEBUG

#ifdef _COROUTINE
    #define __USE_COROUTINE__
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#define __KF_WIN__ 1
#define __KF_LINUX__ 2

#ifdef _WIN32
    #define __KF_SYSTEM__ __KF_WIN__
#else
    #define __KF_SYSTEM__ __KF_LINUX__
#endif // _WIN32

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// win64
#if __KF_SYSTEM__ == __KF_WIN__
    #define __KF_EXPORT__ extern "C"  __declspec(dllexport)

    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS
    #endif // !_CRT_SECURE_NO_WARNINGS

    #ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
        #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #endif // !_WINSOCK_DEPRECATED_NO_WARNINGS

    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT
    #endif
    #define _WIN32_WINNT 0x0600

#endif

// linux
#if __KF_SYSTEM__ == __KF_LINUX__
    #define __KF_EXPORT__ extern "C" __attribute ((visibility("default")))

    #ifndef MAX_PATH
        #define MAX_PATH 256
    #endif
#endif


#endif