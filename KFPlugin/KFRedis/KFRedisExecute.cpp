#include "KFRedisExecute.hpp"
#include "KFRedisFormat.h"

namespace KFrame
{
#define __FREE_REPLY__( p ) if ( p != nullptr ) { freeReplyObject( p ); p = nullptr; }

    /////////////////////////////////////////////////////////////////////////////
    KFRedisExecute::~KFRedisExecute()
    {
        ShutDown();
    }

    int32 KFRedisExecute::Initialize( const std::string& name, const std::string& ip, uint32 port, const std::string& password )
    {
        _name = name;
        _ip = ip;
        _port = port;
        _password = password;

        return TryConnect();
    }

    int32 KFRedisExecute::TryConnect()
    {
        ShutDown();
        __LOG_INFO__( "redis connect[module={} ip={}:{}] start", _name, _ip, _port );

        _redis_context = redisConnect( _ip.c_str(), _port );
        if ( _redis_context == nullptr || _redis_context->err != REDIS_OK )
        {
            return KFEnum::Error;
        }

        auto result = TryAuthPassword( _password );
        if ( result != KFEnum::Ok )
        {
            return result;
        }

        redisEnableKeepAlive( _redis_context );
        __LOG_INFO__( "redis connect[module={} ip={}:{}] ok", _name, _ip, _port );
        return KFEnum::Ok;
    }

    uint32 KFRedisExecute::TryAuthPassword( const std::string& password )
    {
        if ( _password.empty() )
        {
            return KFEnum::Ok;
        }

        auto reply = ( redisReply* )redisCommand( _redis_context, "auth %s", _password.c_str() );
        if ( reply == nullptr )
        {
            __LOG_ERROR__( "redis auth failed", _name, _ip, _port );
            return KFEnum::Error;
        }

        auto type = reply->type;
        __FREE_REPLY__( reply );

        if ( type == REDIS_REPLY_ERROR )
        {
            ShutDown();
            __LOG_ERROR__( "redis password error!" );
            return KFEnum::Error;
        }

        return KFEnum::Ok;
    }

    bool KFRedisExecute::IsDisconnected()
    {
        if ( _redis_context == nullptr ||
                _redis_context->err == REDIS_ERR_IO ||
                _redis_context->err == REDIS_ERR_EOF )
        {
            return true;
        }

        return false;
    }

    void KFRedisExecute::ShutDown()
    {
        if ( _redis_context != nullptr )
        {
            redisFree( _redis_context );
            _redis_context = nullptr;
        }
    }

    redisReply* KFRedisExecute::Execute( const std::string& strsql )
    {
        if ( _redis_context == nullptr )
        {
            __LOG_ERROR__( "_redis_context = nullptr, sql=[{}]", strsql );
            return nullptr;
        }

        auto reply = ( redisReply* )redisCommand( _redis_context, strsql.c_str() );
        if ( reply == nullptr )
        {
            if ( !IsDisconnected() )
            {
                __LOG_ERROR__( "redisreply = nullptr, [{}:{}] sql=[{}]", _redis_context->err, _redis_context->errstr, strsql );
            }
            return nullptr;
        }

        if ( reply->type == REDIS_REPLY_ERROR )
        {
            __LOG_ERROR__( "reply error [{}:{}] sql=[{}]", reply->type, reply->str, strsql );
            __FREE_REPLY__( reply );
            return nullptr;
        }

        return reply;
    }

    redisReply* KFRedisExecute::TryExecute( const std::string& sql )
    {
        auto redis_reply = Execute( sql );
        if ( redis_reply == nullptr )
        {
            if ( IsDisconnected() )
            {
                auto result = TryConnect();
                if ( result == KFEnum::Ok )
                {
                    redis_reply = Execute( sql );
                }
                else
                {
                    __LOG_ERROR__( "redis connect [{}:{}] failed", _ip, _port );
                }
            }
        }

        if ( redis_reply == nullptr )
        {
            if ( _redis_context == nullptr )
            {
                __LOG_ERROR__( "redis=[{}] execute failed", sql );
            }
            else
            {
                __LOG_ERROR__( "redis=[{}] execute failed[{}:{}]", sql, _redis_context->err, _redis_context->errstr );
            }
        }

        return redis_reply;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< voidptr >::UniqueType KFRedisWriteExecute::WriteVoid( const std::string& strsql )
    {
        __NEW_RESULT__( voidptr );
        auto redisreply = TryExecute( strsql );
        if ( redisreply == nullptr )
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< uint64 >::UniqueType KFRedisWriteExecute::WriteUInt64( const std::string& strsql )
    {
        __NEW_RESULT__( uint64 );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->type == REDIS_REPLY_INTEGER )
            {
                result->_value = redisreply->integer;
            }
            else if ( redisreply->type == REDIS_REPLY_STRING )
            {
                if ( !KFRedisFormat::IsEmptyString( redisreply->str ) )
                {
                    result->_value = __TO_UINT64__( redisreply->str );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< std::string >::UniqueType KFRedisWriteExecute::WriteString( const std::string& strsql )
    {
        __NEW_RESULT__( std::string );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            result->_value = ( KFRedisFormat::IsEmptyString( redisreply->str ) ? _invalid_string : redisreply->str );
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redisreply );
        return result;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< uint64 >::UniqueType KFRedisReadExecute::ReadUInt64( const std::string& strsql )
    {
        __NEW_RESULT__( uint64 );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->type == REDIS_REPLY_INTEGER )
            {
                result->_value = redisreply->integer;
            }
            else if ( redisreply->type == REDIS_REPLY_STRING )
            {
                if ( !KFRedisFormat::IsEmptyString( redisreply->str ) )
                {
                    result->_value = __TO_UINT64__( redisreply->str );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< std::string >::UniqueType KFRedisReadExecute::ReadString( const std::string& strsql )
    {
        __NEW_RESULT__( std::string );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            result->_value = ( KFRedisFormat::IsEmptyString( redisreply->str ) ? _invalid_string : redisreply->str );
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringMap >::UniqueType KFRedisReadExecute::ReadMap( const std::string& strsql )
    {
        __NEW_RESULT__( StringMap );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0u; i < redisreply->elements; i += 2 )
            {
                auto keyelement = redisreply->element[ i ];
                auto valueelement = redisreply->element[ i + 1 ];

                if ( !KFRedisFormat::IsEmptyString( keyelement->str ) &&
                        !KFRedisFormat::IsEmptyString( valueelement->str ) )
                {
                    result->_value[ keyelement->str ] = valueelement->str;
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringPair >::UniqueType KFRedisReadExecute::ReadPair( const std::string& strsql )
    {
        __NEW_RESULT__( StringPair );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->elements >= 2 )
            {
                result->_value = StringPair( redisreply->element[ 0 ]->str, redisreply->element[ 1 ]->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringList >::UniqueType KFRedisReadExecute::ReadList( const std::string& strsql )
    {
        __NEW_RESULT__( StringList );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                result->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringVector >::UniqueType KFRedisReadExecute::ReadVector( const std::string& strsql )
    {
        __NEW_RESULT__( StringVector );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                result->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringMapList >::UniqueType KFRedisReadExecute::ReadMapList( const std::string& strsql )
    {
        __NEW_RESULT__( StringMapList );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            //for ( size_t i = 0; i < redisreply->elements; ++i )
            //{
            //	auto element = redisreply->element[ i ];
            //	kfresult->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            //}
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

    KFResult< StringPairList >::UniqueType KFRedisReadExecute::ReadPairList( const std::string& strsql )
    {
        __NEW_RESULT__( StringPairList );
        auto redisreply = TryExecute( strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0u; i < redisreply->elements; i += 2 )
            {
                auto keyelement = redisreply->element[ i ];
                auto valueelement = redisreply->element[ i + 1 ];

                if ( !KFRedisFormat::IsEmptyString( keyelement->str ) &&
                        !KFRedisFormat::IsEmptyString( valueelement->str ) )
                {
                    result->_value.emplace_back( StringPair( keyelement->str, valueelement->str ) );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redisreply );
        return result;
    }

}