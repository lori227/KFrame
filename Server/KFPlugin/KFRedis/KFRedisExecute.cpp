#include "KFRedisExecute.h"

namespace KFrame
{
#define __FREE_REPLY__( p ) if ( p != nullptr ) { freeReplyObject( p ); p = nullptr; }

    /////////////////////////////////////////////////////////////////////////////
    KFRedisExecute::KFRedisExecute()
    {
        _index = 0;
        _port = 0;
        _redis_context = nullptr;
    }

    KFRedisExecute::~KFRedisExecute()
    {

    }

    int32 KFRedisExecute::Initialize( const std::string& ip, uint32 port, const std::string& password )
    {
        _ip = ip;
        _port = port;
        _password = password;

        return TryConnect();
    }


    int32 KFRedisExecute::TryConnect()
    {
        ShutDown();

        _redis_context = redisConnect( _ip.c_str(), _port );
        if ( _redis_context->err != REDIS_OK )
        {
            return _redis_context->err;
        }

        redisEnableKeepAlive( _redis_context );

        if ( !_password.empty() )
        {
            auto strsql = __FORMAT__( "auth {}", _password );
            auto reply = Execute( strsql );
            if ( reply == nullptr )
            {
                return KFEnum::Error;
            }

            __FREE_REPLY__( reply );
        }

        return KFEnum::Ok;
    }

    bool KFRedisExecute::IsDisconnected()
    {
        if ( _redis_context->err == REDIS_ERR_EOF || _redis_context->err == REDIS_ERR_IO )
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

    void KFRedisExecute::SelectIndex( uint32 index )
    {
        if ( index == _index )
        {
            return;
        }

        auto strsql = __FORMAT__( "select {}", _index );
        auto kfresult = UpdateExecute( __FUNC_LINE__, strsql );
        if ( kfresult->IsOk() )
        {
            _index = index;
        }
    }


    redisReply* KFRedisExecute::Execute( const std::string& strsql )
    {
        auto reply = ( redisReply* )redisCommand( _redis_context, strsql.c_str() );
        if ( reply == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::Sql, "redisreply = nullptr, [{}:{}]!",
                           _redis_context->err, _redis_context->errstr );
            return nullptr;
        }

        if ( reply->type == REDIS_REPLY_ERROR )
        {
            __LOG_ERROR__( KFLogEnum::Sql, "reply error [{}:{}]!",
                           _redis_context->err, _redis_context->errstr );
            __FREE_REPLY__( reply );
            return nullptr;
        }

        return reply;
    }

    redisReply* KFRedisExecute::TryExecute( KFBaseResult* kfresult, const char* function, uint32 line, const std::string& strsql )
    {
        auto redisreply = Execute( strsql );
        if ( redisreply == nullptr )
        {
            if ( IsDisconnected() )
            {
                TryConnect();
                redisreply = Execute( strsql );
            }
        }

        if ( redisreply != nullptr )
        {
            kfresult->SetResult( KFEnum::Ok );
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
            __LOG_ERROR_FUNCTION__( KFLogEnum::Sql, function, line, "redis[{}] execute error=[{}:{}]!",
                                    strsql, _redis_context->err, _redis_context->errstr );
        }

        return redisreply;
    }

    KFResult< voidptr >* KFRedisExecute::UpdateExecute( const char* function, uint32 line, const std::string& strsql )
    {
        auto redisreply = TryExecute( &_void_result, function, line, strsql );
        __FREE_REPLY__( redisreply );
        return &_void_result;
    }

    KFResult< uint32 >* KFRedisExecute::UInt32Execute( const char* function, uint32 line, const std::string& strsql )
    {
        _uint32_result._value = _invalid_int;
        auto redisreply = TryExecute( &_uint32_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->str != nullptr )
            {
                _uint32_result._value = KFUtility::ToValue< uint32 >( redisreply->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return &_uint32_result;
    }

    KFResult< std::string >* KFRedisExecute::StringExecute( const char* function, uint32 line, const std::string& strsql )
    {
        _string_result._value.clear();
        auto redisreply = TryExecute( &_string_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            _string_result._value = ( redisreply->str == nullptr ? _invalid_str : redisreply->str );
        }

        __FREE_REPLY__( redisreply );
        return &_string_result;
    }

    KFResult< uint64 >* KFRedisExecute::UInt64Execute( const char* function, uint32 line, const std::string& strsql )
    {
        _uint64_result._value = _invalid_int;
        auto redisreply = TryExecute( &_uint64_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            _uint64_result._value = redisreply->integer;
        }

        __FREE_REPLY__( redisreply );
        return &_uint64_result;
    }

    KFResult< MapString >* KFRedisExecute::MapExecute( const char* function, uint32 line, const std::string& strsql )
    {
        _map_result._value.clear();
        auto redisreply = TryExecute( &_map_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; i += 2 )
            {
                auto keyelement = redisreply->element[ i ];
                auto valueelement = redisreply->element[ i + 1 ];

                std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
                std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
                if ( key != _invalid_str )
                {
                    _map_result._value[ key ] = value;
                }
            }
        }

        __FREE_REPLY__( redisreply );
        return &_map_result;
    }

    KFResult< GreaterMapString >* KFRedisExecute::GreaterMapExecute( const char* function, uint32 line, const std::string& strsql )
    {
        _greater_map_result._value.clear();
        auto redisreply = TryExecute( &_greater_map_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; i += 2 )
            {
                auto keyelement = redisreply->element[ i ];
                auto valueelement = redisreply->element[ i + 1 ];

                std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
                std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
                if ( key != _invalid_str )
                {
                    _greater_map_result._value[ key ] = value;
                }
            }
        }

        __FREE_REPLY__( redisreply );
        return &_greater_map_result;
    }

    KFResult< VectorString >* KFRedisExecute::VectorExecute( const char* function, uint32 line, const std::string& strsql )
    {
        _vector_result._value.clear();
        auto redisreply = TryExecute( &_vector_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                _vector_result._value.push_back( element->str == nullptr ? _invalid_str : element->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return &_vector_result;
    }

    KFResult< ListString >* KFRedisExecute::ListExecute( const char* function, uint32 line, const std::string& strsql )
    {
        _list_result._value.clear();
        auto redisreply = TryExecute( &_list_result, function, line, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                _list_result._value.push_back( element->str == nullptr ? _invalid_str : element->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return &_list_result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRedisExecute::AppendCommand( const std::string& strsql )
    {
        _commands.push_back( strsql );
    }

#define __REDIS_GET_REPLY__( rediscontext, reply ) \
    redisGetReply( rediscontext, ( void** )&reply )
    //__COROUTINE__( redisGetReply( rediscontext, ( void** )&reply ) )

    // todo: 发生错误是否需要回滚
    KFResult< voidptr >* KFRedisExecute::Pipeline( const char* function, uint32 line )
    {
        _void_result.SetResult( KFEnum::Ok );

        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                _void_result.SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : _commands )
        {
            redisReply* reply = nullptr;
            __REDIS_GET_REPLY__( _redis_context, reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    __REDIS_GET_REPLY__( _redis_context, reply );
                }
            }

            if ( reply != nullptr )
            {
                freeReplyObject( reply );
            }
            else
            {
                _void_result.SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return &_void_result;
    }

    KFResult< ListString >* KFRedisExecute::ListPipelineExecute( const char* function, uint32 line )
    {
        _list_result.SetResult( KFEnum::Ok );
        _list_result._value.clear();

        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                _list_result.SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : _commands )
        {
            redisReply* reply = nullptr;
            __REDIS_GET_REPLY__( _redis_context, reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    __REDIS_GET_REPLY__( _redis_context, reply );
                }
            }

            if ( reply != nullptr )
            {
                _list_result._value.push_back( reply->str == nullptr ? _invalid_str : reply->str );
                freeReplyObject( reply );
            }
            else
            {
                _list_result.SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return &_list_result;
    }

    KFResult< std::list< MapString > >* KFRedisExecute::ListMapPipelineExecute( const char* function, uint32 line )
    {
        _list_map_result.SetResult( KFEnum::Ok );
        _list_map_result._value.clear();

        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                _list_map_result.SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : _commands )
        {
            redisReply* reply = nullptr;
            __REDIS_GET_REPLY__( _redis_context, reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    __REDIS_GET_REPLY__( _redis_context, reply );
                }
            }

            if ( reply != nullptr )
            {
                MapString values;
                for ( size_t i = 0; i < reply->elements; i += 2 )
                {
                    auto keyelement = reply->element[ i ];
                    auto valueelement = reply->element[ i + 1 ];

                    std::string key = ( keyelement->str == nullptr ? _invalid_str : keyelement->str );
                    std::string value = ( valueelement->str == nullptr ? _invalid_str : valueelement->str );
                    if ( key != _invalid_str )
                    {
                        values[ key ] = value;
                    }
                }

                _list_map_result._value.push_back( values );
                freeReplyObject( reply );
            }
            else
            {
                _list_map_result.SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return &_list_map_result;
    }


}