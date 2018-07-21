#include "KFHiRedis.h"
#include "hiredis/hiredis.h"
#include "KFRedisException.h"
#ifdef _WIN32
    #include <WinSock2.h>
#endif // _WIN32


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    KFHiRedis::KFHiRedis()
    {
        _port = 0;
        _redis_context = nullptr;
    }

    KFHiRedis::~KFHiRedis()
    {
        //if ( _redis_context != nullptr )
        //{
        //	redisFree( _redis_context );
        //	_redis_context = nullptr;
        //}
    }

    bool KFHiRedis::IsDisconnected()
    {
        if ( _redis_context == nullptr )
        {
            return true;
        }

        if ( _redis_context->err == REDIS_ERR_EOF ||
                _redis_context->err == REDIS_ERR_IO )
        {
            return true;
        }

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int32 KFHiRedis::Connect( const char* ip, int32 port, const char* password )
    {
        _ip = ip;
        _port = port;
        _password = password;

#ifdef _WIN32
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;
        _redis_context = redisConnectWithTimeout( ip, port, timeout );
#else
        _redis_context = redisConnect( ip, port );
#endif // _WIN32

        if ( _redis_context->err != REDIS_OK )
        {
            return _redis_context->err;
        }

        redisEnableKeepAlive( _redis_context );

        try
        {
            if ( password[ 0 ] != 0 )
            {
                char command[ 56 ] = { 0 };
                sprintf( command, "auth %s", password );
                auto redisreply = Execute( REDIS_REPLY_STATUS, command );
                freeReplyObject( redisreply );
            }
        }
        catch ( KFRedisException& )
        {
            return 2;
        }

        return 1;
    }

    int32 KFHiRedis::ReConnect()
    {
        if ( _redis_context != nullptr )
        {
            redisFree( _redis_context );
            _redis_context = nullptr;
        }

        return Connect( _ip.c_str(), _port, _password.c_str() );
    }

    void KFHiRedis::ShutDown()
    {
        if ( _redis_context != nullptr )
        {
            redisFree( _redis_context );
            _redis_context = nullptr;
        }
    }

    redisReply* KFHiRedis::Execute( int32 type, const char* command )
    {
        redisReply* reply = ( redisReply* ) redisCommand( _redis_context, command );
        if ( reply == nullptr )
        {
            throw KFRedisException( command, _redis_context->err, _redis_context->errstr );
        }

        if ( reply->type == REDIS_REPLY_ERROR )
        {
            std::string error;
            if ( reply->str != nullptr )
            {
                error = reply->str;
            }

            freeReplyObject( reply );
            throw KFRedisException( command, _redis_context->flags, error.c_str() );
        }

        return reply;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template<>
    bool KFHiRedis::Execute<>( uint32& value, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_STATUS, command );
        freeReplyObject( reply );
        return true;
    }

    template<>
    bool KFHiRedis::Execute<>( std::string& value, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_STRING, command );

        value = ( reply->str == nullptr ? "" : reply->str );
        freeReplyObject( reply );
        return true;
    }

    template<>
    bool KFHiRedis::Execute<>( uint64& value, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_INTEGER, command );

        value = reply->integer;
        freeReplyObject( reply );
        return true;
    }

    template<>
    bool KFHiRedis::Execute<>( VectorString& result, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_ARRAY, command );

        for ( size_t i = 0; i < reply->elements; ++i )
        {
            auto element = reply->element[ i ];

            std::string value = ( element->str == nullptr ? "" : element->str );
            result.push_back( value );
        }

        freeReplyObject( reply );
        return true;
    }

    template<>
    bool KFHiRedis::Execute<>( MapString& result, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_ARRAY, command );

        for ( size_t i = 0; i < reply->elements; i += 2 )
        {
            auto keyelement = reply->element[ i ];
            auto valueelement = reply->element[ i + 1 ];

            std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
            std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
            if ( key == _invalid_str )
            {
                continue;
            }

            result[ key ] = value;
        }

        freeReplyObject( reply );
        return true;
    }

    template<>
    bool KFHiRedis::Execute<>( GreaterMapString& result, const char* command )
    {
        redisReply* reply = Execute( REDIS_REPLY_ARRAY, command );

        for ( size_t i = 0; i < reply->elements; i += 2 )
        {
            auto keyelement = reply->element[ i ];
            auto valueelement = reply->element[ i + 1 ];

            std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
            std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
            if ( key == _invalid_str )
            {
                continue;
            }

            result[ key ] = value;
        }

        freeReplyObject( reply );
        return true;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool KFHiRedis::PipelineExecute( const ListString& commands )
    {
        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisAppendCommand" );
            }
        }

        redisReply* reply = nullptr;
        for ( auto& command : commands )
        {
            auto result = redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisGetReply" );
            }

            freeReplyObject( reply );
            reply = nullptr;
        }

        return true;
    }

    bool KFHiRedis::PipelineExecute( const ListString& commands, MapString& values )
    {
        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisAppendCommand" );
            }
        }

        redisReply* reply = nullptr;
        for ( auto& command : commands )
        {
            auto result = redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisGetReply" );
            }

            for ( size_t i = 0; i < reply->elements; i += 2 )
            {
                auto keyelement = reply->element[ i ];
                auto valueelement = reply->element[ i + 1 ];

                std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
                std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
                if ( key == _invalid_str )
                {
                    continue;
                }

                values[ key ] = value;
            }

            freeReplyObject( reply );
            reply = nullptr;
        }

        return true;
    }

    bool KFHiRedis::PipelineExecute( const ListString& commands, VectorString& values )
    {
        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisAppendCommand" );
            }
        }

        redisReply* reply = nullptr;
        for ( auto& command : commands )
        {
            auto result = redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                throw KFRedisException( command.c_str(), _redis_context->flags, "redisGetReply" );
            }

            if ( reply->elements != 0 )
            {
                for ( size_t i = 0; i < reply->elements; ++i )
                {
                    auto element = reply->element[ i ];

                    std::string value = ( element->str == nullptr ? _invalid_str : element->str );
                    values.push_back( value );
                }
            }

            freeReplyObject( reply );
            reply = nullptr;
        }

        return true;
    }
}