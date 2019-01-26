#ifndef __KF_FIELD_H__
#define __KF_FIELD_H__

namespace KFrame
{
#define __STRING_DEFINE__( name )  static std::string _##name = #name;
#define __KF_STRING__( name )  KFField::_##name

    namespace KFField
    {
        __STRING_DEFINE__( id );
        __STRING_DEFINE__( master );
        __STRING_DEFINE__( proxy );
        __STRING_DEFINE__( shard );
        __STRING_DEFINE__( route );
        __STRING_DEFINE__( ip );
        __STRING_DEFINE__( port );
        __STRING_DEFINE__( daemon );
        __STRING_DEFINE__( appname );
        __STRING_DEFINE__( apptype );
        __STRING_DEFINE__( appid );
        __STRING_DEFINE__( service );
        __STRING_DEFINE__( log );
        __STRING_DEFINE__( address );

    }
}

#endif
