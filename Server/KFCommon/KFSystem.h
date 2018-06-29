#ifndef __KF_SYSTEM_H__
#define __KF_SYSTEM_H__


#ifdef _DEBUG
    #define __KF_DEBUG__
#else
    #define __KF_RELEASE__
#endif // _DEBUG

#ifndef POCO_STATIC
    #define POCO_STATIC
#endif // !1

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


#endif

// linux
#if __KF_SYSTEM__ == __KF_LINUX__
    #define __KF_EXPORT__ extern "C" __attribute ((visibility("default")))

#endif


#endif