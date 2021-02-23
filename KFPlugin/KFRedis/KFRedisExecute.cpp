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

    redisReply* KFRedisExecute::Execute( const std::string& sql )
    {
        if ( _redis_context == nullptr )
        {
            __LOG_ERROR__( "_redis_context = nullptr, sql=[{}]", sql );
            return nullptr;
        }

        auto reply = ( redisReply* )redisCommand( _redis_context, sql.c_str() );
        if ( reply == nullptr )
        {
            if ( !IsDisconnected() )
            {
                __LOG_ERROR__( "reply = nullptr, [{}:{}] sql=[{}]", _redis_context->err, _redis_context->errstr, sql );
            }
            return nullptr;
        }

        if ( reply->type == REDIS_REPLY_ERROR )
        {
            __LOG_ERROR__( "reply error [{}:{}] sql=[{}]", reply->type, reply->str, sql );
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
    KFResult<voidptr>::UniqueType KFRedisWriteExecute::WriteVoid( const std::string& sql )
    {
        __NEW_RESULT__( voidptr );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply == nullptr )
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<uint64>::UniqueType KFRedisWriteExecute::WriteUInt64( const std::string& sql )
    {
        __NEW_RESULT__( uint64 );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            if ( redis_reply->type == REDIS_REPLY_INTEGER )
            {
                result->_value = redis_reply->integer;
            }
            else if ( redis_reply->type == REDIS_REPLY_STRING )
            {
                if ( !KFRedisFormat::IsEmptyString( redis_reply->str ) )
                {
                    result->_value = __TO_UINT64__( redis_reply->str );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<std::string>::UniqueType KFRedisWriteExecute::WriteString( const std::string& sql )
    {
        __NEW_RESULT__( std::string );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            result->_value = ( KFRedisFormat::IsEmptyString( redis_reply->str ) ? _invalid_string : redis_reply->str );
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }
        __FREE_REPLY__( redis_reply );
        return result;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult<uint64>::UniqueType KFRedisReadExecute::ReadUInt64( const std::string& sql )
    {
        __NEW_RESULT__( uint64 );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            if ( redis_reply->type == REDIS_REPLY_INTEGER )
            {
                result->_value = redis_reply->integer;
            }
            else if ( redis_reply->type == REDIS_REPLY_STRING )
            {
                if ( !KFRedisFormat::IsEmptyString( redis_reply->str ) )
                {
                    result->_value = __TO_UINT64__( redis_reply->str );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<std::string>::UniqueType KFRedisReadExecute::ReadString( const std::string& sql )
    {
        __NEW_RESULT__( std::string );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            result->_value = ( KFRedisFormat::IsEmptyString( redis_reply->str ) ? _invalid_string : redis_reply->str );
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringMap>::UniqueType KFRedisReadExecute::ReadMap( const std::string& sql )
    {
        __NEW_RESULT__( StringMap );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            for ( size_t i = 0u; i < redis_reply->elements; i += 2 )
            {
                auto key_element = redis_reply->element[ i ];
                auto value_element = redis_reply->element[ i + 1 ];

                if ( !KFRedisFormat::IsEmptyString( key_element->str ) &&
                        !KFRedisFormat::IsEmptyString( value_element->str ) )
                {
                    result->_value[ key_element->str ] = value_element->str;
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringPair>::UniqueType KFRedisReadExecute::ReadPair( const std::string& sql )
    {
        __NEW_RESULT__( StringPair );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            if ( redis_reply->elements >= 2 )
            {
                result->_value = StringPair( redis_reply->element[ 0 ]->str, redis_reply->element[ 1 ]->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringList>::UniqueType KFRedisReadExecute::ReadList( const std::string& sql )
    {
        __NEW_RESULT__( StringList );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            for ( size_t i = 0; i < redis_reply->elements; ++i )
            {
                auto element = redis_reply->element[ i ];
                result->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringVector>::UniqueType KFRedisReadExecute::ReadVector( const std::string& sql )
    {
        __NEW_RESULT__( StringVector );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            for ( size_t i = 0; i < redis_reply->elements; ++i )
            {
                auto element = redis_reply->element[ i ];
                result->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringMapList>::UniqueType KFRedisReadExecute::ReadMapList( const std::string& sql )
    {
        __NEW_RESULT__( StringMapList );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            //for ( size_t i = 0; i < redis_reply->elements; ++i )
            //{
            //	auto element = redis_reply->element[ i ];
            //	result->_value.push_back( KFRedisFormat::IsEmptyString( element->str ) ? _invalid_string : element->str );
            //}
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

    KFResult<StringPairList>::UniqueType KFRedisReadExecute::ReadPairList( const std::string& sql )
    {
        __NEW_RESULT__( StringPairList );
        auto redis_reply = TryExecute( sql );
        if ( redis_reply != nullptr )
        {
            for ( size_t i = 0u; i < redis_reply->elements; i += 2 )
            {
                auto key_element = redis_reply->element[ i ];
                auto value_element = redis_reply->element[ i + 1 ];

                if ( !KFRedisFormat::IsEmptyString( key_element->str ) &&
                        !KFRedisFormat::IsEmptyString( value_element->str ) )
                {
                    result->_value.emplace_back( StringPair( key_element->str, value_element->str ) );
                }
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        __FREE_REPLY__( redis_reply );
        return result;
    }

}