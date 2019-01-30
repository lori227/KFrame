#ifndef __KF_FIELD_H__
#define __KF_FIELD_H__

namespace KFrame
{
#define __STRING_DEFINE__( name )  static std::string _##name = #name;
#define __KF_STRING__( name )  KFField::_##name

    namespace KFField
    {
        // frame
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
        __STRING_DEFINE__( player );
        __STRING_DEFINE__( gateid );
        __STRING_DEFINE__( channel );
        __STRING_DEFINE__( account );
        __STRING_DEFINE__( accountid );
        __STRING_DEFINE__( basic );
        __STRING_DEFINE__( serverid );
        __STRING_DEFINE__( name );
        __STRING_DEFINE__( onlinetime );
        __STRING_DEFINE__( offlinetime );
        __STRING_DEFINE__( totaltime );
        __STRING_DEFINE__( birthday );
        __STRING_DEFINE__( clusterkey );
        __STRING_DEFINE__( clustername );

        // midware
        __STRING_DEFINE__( zoneregister );
        __STRING_DEFINE__( zoneupdate );
        __STRING_DEFINE__( zonelist );
        __STRING_DEFINE__( zoneip );
        __STRING_DEFINE__( zoneurl );
        __STRING_DEFINE__( zonelogicid );
        __STRING_DEFINE__( auth );
        __STRING_DEFINE__( verify );
        __STRING_DEFINE__( activation );
        __STRING_DEFINE__( onlinedata );
        __STRING_DEFINE__( ban );
        __STRING_DEFINE__( banip );
        __STRING_DEFINE__( banplayerid );
        __STRING_DEFINE__( banaccountid );
        __STRING_DEFINE__( bantime );
        __STRING_DEFINE__( unban );
        __STRING_DEFINE__( queryban );
        __STRING_DEFINE__( queryaccount );
        __STRING_DEFINE__( extend );
        __STRING_DEFINE__( user );
        __STRING_DEFINE__( playeridcreate );
        __STRING_DEFINE__( url );
        __STRING_DEFINE__( tokenexpiretime );
        __STRING_DEFINE__( token );
        __STRING_DEFINE__( openid );
        __STRING_DEFINE__( channeldata );
        __STRING_DEFINE__( count );
        __STRING_DEFINE__( code );
        __STRING_DEFINE__( verifyactivationcode );
        __STRING_DEFINE__( public );
        __STRING_DEFINE__( openactivation );
        __STRING_DEFINE__( activationcode );
        __STRING_DEFINE__( online );
        __STRING_DEFINE__( kickonline );
        __STRING_DEFINE__( accountlist );
        __STRING_DEFINE__( accountmake );
        __STRING_DEFINE__( access_token );
        __STRING_DEFINE__( expires_in );
        __STRING_DEFINE__( scope );
        __STRING_DEFINE__( errcode );
        __STRING_DEFINE__( refresh_token );
        __STRING_DEFINE__( response );
        __STRING_DEFINE__( error );
        __STRING_DEFINE__( params );
        __STRING_DEFINE__( steamid );
        __STRING_DEFINE__( gate );
        __STRING_DEFINE__( world );
        __STRING_DEFINE__( game );
        __STRING_DEFINE__( login );
        __STRING_DEFINE__( data );
        __STRING_DEFINE__( sessionid );
        __STRING_DEFINE__( playerid );
        __STRING_DEFINE__( adddata );
        __STRING_DEFINE__( setdata );
        __STRING_DEFINE__( decdata );
        __STRING_DEFINE__( item );
        __STRING_DEFINE__( playernamelength );
        __STRING_DEFINE__( sex );
        __STRING_DEFINE__( achieve );
        __STRING_DEFINE__( level );
        __STRING_DEFINE__( task );
        __STRING_DEFINE__( activity );
        __STRING_DEFINE__( received );
        __STRING_DEFINE__( hero );
        __STRING_DEFINE__( skin );
        __STRING_DEFINE__( signin );
        __STRING_DEFINE__( sevenday );
        __STRING_DEFINE__( sevenreward );
        __STRING_DEFINE__( note );
        __STRING_DEFINE__( luafile );
        __STRING_DEFINE__( luafunction );
        __STRING_DEFINE__( mail );
        __STRING_DEFINE__( reward );
        __STRING_DEFINE__( validtime );
        __STRING_DEFINE__( configid );
        __STRING_DEFINE__( content );
        __STRING_DEFINE__( title );
        __STRING_DEFINE__( type );
        __STRING_DEFINE__( flag );
        __STRING_DEFINE__( sendid );
        __STRING_DEFINE__( sendname );
        __STRING_DEFINE__( maxmailid );
        __STRING_DEFINE__( sendtime );
        __STRING_DEFINE__( globalmail );
        __STRING_DEFINE__( mailinfo );
        __STRING_DEFINE__( gmmailid );
        __STRING_DEFINE__( mailzonelist );
        __STRING_DEFINE__( maillist );
        __STRING_DEFINE__( mailidcreater );
        __STRING_DEFINE__( mailid );
        __STRING_DEFINE__( addmail );
        __STRING_DEFINE__( delmail );


    }
}

#endif
