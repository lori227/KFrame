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

        _result_queue_list.push_back( &_void_result_queue );
        _result_queue_list.push_back( &_uint32_result_queue );
        _result_queue_list.push_back( &_uint64_result_queue );
        _result_queue_list.push_back( &_string_result_queue );
        _result_queue_list.push_back( &_map_result_queue );
        _result_queue_list.push_back( &_vector_result_queue );
        _result_queue_list.push_back( &_list_result_queue );
        _result_queue_list.push_back( &_list_map_result_queue );
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

    void KFRedisExecute::Run()
    {
        // 释放结果内存
        for ( auto kfresultqueue : _result_queue_list )
        {
            kfresultqueue->Free();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    void KFRedisExecute::SelectIndex( uint32 index )
    {
        if ( index == _index )
        {
            return;
        }

        auto strsql = __FORMAT__( "select {}", _index );
        auto kfresult = UpdateExecute( strsql );
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

    redisReply* KFRedisExecute::TryExecute( KFBaseResult* kfresult, const std::string& strsql )
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
            __LOG_ERROR__( KFLogEnum::Sql, "redis[{}] execute error=[{}:{}]!",
                           strsql, _redis_context->err, _redis_context->errstr );
        }

        return redisreply;
    }

    KFResult< voidptr >* KFRedisExecute::UpdateExecute( const std::string& strsql )
    {
        auto kfresult = _void_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< uint32 >* KFRedisExecute::UInt32Execute( const std::string& strsql )
    {
        auto kfresult = _uint32_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->str != nullptr )
            {
                kfresult->_value = KFUtility::ToValue< uint32 >( redisreply->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< std::string >* KFRedisExecute::StringExecute( const std::string& strsql )
    {
        auto kfresult = _string_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            kfresult->_value = ( redisreply->str == nullptr ? _invalid_str : redisreply->str );
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< uint64 >* KFRedisExecute::UInt64Execute( const std::string& strsql )
    {
        auto kfresult = _uint64_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            kfresult->_value = redisreply->integer;
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< MapString >* KFRedisExecute::MapExecute( const std::string& strsql )
    {
        auto kfresult = _map_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
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
                    kfresult->_value[ key ] = value;
                }
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< VectorString >* KFRedisExecute::VectorExecute( const std::string& strsql )
    {
        auto kfresult = _vector_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                kfresult->_value.push_back( element->str == nullptr ? _invalid_str : element->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< ListString >* KFRedisExecute::ListExecute( const std::string& strsql )
    {
        auto kfresult = _list_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0; i < redisreply->elements; ++i )
            {
                auto element = redisreply->element[ i ];
                kfresult->_value.push_back( element->str == nullptr ? _invalid_str : element->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRedisExecute::AppendCommand( const std::string& strsql )
    {
        _commands.push_back( strsql );
    }

#define __REDIS_GET_REPLY__( rediscontext, reply ) \
    redisGetReply( rediscontext, ( void** )&reply )

    // todo: 发生错误是否需要回滚
    KFResult< voidptr >* KFRedisExecute::Pipeline()
    {
        auto kfresult = _void_result_queue.Alloc();
        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
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
                kfresult->SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return kfresult;
    }

    KFResult< ListString >* KFRedisExecute::ListPipelineExecute()
    {
        auto kfresult = _list_result_queue.Alloc();
        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
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
                kfresult->_value.push_back( reply->str == nullptr ? _invalid_str : reply->str );
                freeReplyObject( reply );
            }
            else
            {
                kfresult->SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return kfresult;
    }

    KFResult< std::list< MapString > >* KFRedisExecute::ListMapPipelineExecute()
    {
        auto kfresult = _list_map_result_queue.Alloc();

        for ( auto& command : _commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
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

                kfresult->_value.push_back( values );
                freeReplyObject( reply );

            }
            else
            {
                kfresult->SetResult( KFEnum::Error );
            }
        }

        _commands.clear();
        return kfresult;
    }


}