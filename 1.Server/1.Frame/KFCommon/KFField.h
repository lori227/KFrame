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
        __STRING_DEFINE__( apppath );
        __STRING_DEFINE__( appfile );
        __STRING_DEFINE__( appconfig );
        __STRING_DEFINE__( appid );
        __STRING_DEFINE__( service );
        __STRING_DEFINE__( log );
        __STRING_DEFINE__( address );
        __STRING_DEFINE__( deploy );
        __STRING_DEFINE__( deploypath );
        __STRING_DEFINE__( localip );
        __STRING_DEFINE__( machine );
        __STRING_DEFINE__( status );
        __STRING_DEFINE__( agentid );
        __STRING_DEFINE__( command );
        __STRING_DEFINE__( value );
        __STRING_DEFINE__( zone );
        __STRING_DEFINE__( zoneid );
        __STRING_DEFINE__( logurl );
        __STRING_DEFINE__( scheduletime );
        __STRING_DEFINE__( msg );
        __STRING_DEFINE__( startup );
        __STRING_DEFINE__( debug );
        __STRING_DEFINE__( logtype );
        __STRING_DEFINE__( shutdown );
        __STRING_DEFINE__( strappid );
        __STRING_DEFINE__( agent );
        __STRING_DEFINE__( server );
        __STRING_DEFINE__( launch );
        __STRING_DEFINE__( process );
        __STRING_DEFINE__( version );
        __STRING_DEFINE__( wget );
        __STRING_DEFINE__( kill );
        __STRING_DEFINE__( downfile );
        __STRING_DEFINE__( reloadconfig );
        __STRING_DEFINE__( loadconfig );
        __STRING_DEFINE__( loadscript );
        __STRING_DEFINE__( time );
        __STRING_DEFINE__( restart );
        __STRING_DEFINE__( reloadscript );
        __STRING_DEFINE__( cleantask );
        __STRING_DEFINE__( loglevel );
        __STRING_DEFINE__( logmemory );


    }
}

#endif
