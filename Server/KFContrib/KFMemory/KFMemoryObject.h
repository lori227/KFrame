#ifndef __KF_MEMORY_OBJECT_H__
#define __KF_MEMORY_OBJECT_H__

#include "KFMalloc.h"

namespace KFrame
{
#define __MT_CLASS__( name ) \
    class name : public KFMTObject

#define __ST_CLASS__( name ) \
    class name : public KFSTObject

#define __KF_CREATE_BATCH__( name, batch, ... ) \
    name::CreateObject< name >( __FUNCTION_LINE__, batch, ##__VA_ARGS__ )

#define __KF_CREATE__( name, ... ) \
    name::CreateObject< name >( __FUNCTION_LINE__, 30, ##__VA_ARGS__ )

#define  __KF_DESTROY__( name, object ) \
    name::DestoryObject< name >( object, __FUNCTION_LINE__ )
    //////////////////////////////////////////////////////////////////
    class KFMTObject
    {
    public:
        virtual ~KFMTObject() {};

        template< class T, class ... P >
        inline static T* CreateObject( const char* function, uint32 line, uint32 batch, P&& ...params )
        {
            return KFMalloc::Instance()->MTCreate< T >( function, line, batch, std::forward<P>( params )... );
        }

        template< class T >
        inline static void DestoryObject( T* object, const char* function, uint32 line )
        {
            KFMalloc::Instance()->MTDestroy( object, function, line );
        }
    };

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    class KFSTObject
    {
    public:
        virtual ~KFSTObject() {};

        template< class T, class ... P >
        inline static T* CreateObject( const char* function, uint32 line, uint32 batch, P&& ...params )
        {
            return KFMalloc::Instance()->STCreate< T >( function, line, batch, std::forward<P>( params )... );
        }

        template< class T >
        inline static void DestoryObject( T* object, const char* function, uint32 line )
        {
            KFMalloc::Instance()->STDestroy( object, function, line );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////


}

#endif