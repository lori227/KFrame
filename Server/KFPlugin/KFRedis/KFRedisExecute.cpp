#include "KFRedisExecute.h"

namespace KFrame
{
#define __FREE_REPLY__( p ) if ( p != nullptr ) { freeReplyObject( p ); p = nullptr; }

    /////////////////////////////////////////////////////////////////////////////
    KFRedisExecute::KFRedisExecute()
    {
        _port = 0;
        _redis_context = nullptr;
    }

    KFRedisExecute::~KFRedisExecute()
    {
        ShutDown();
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
        if ( _redis_context == nullptr || _redis_context->err != REDIS_OK )
        {
            return KFEnum::Error;
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

        __LOG_INFO__( "redis connect [{}:{}] ok!", _ip, _port );
        return KFEnum::Ok;
    }

    bool KFRedisExecute::IsDisconnected()
    {
        if ( _redis_context == nullptr ||
                _redis_context->err == REDIS_ERR_EOF ||
                _redis_context->err == REDIS_ERR_IO )
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

    redisReply* KFRedisExecute::Execute( const std::string& strsql )
    {
        if ( _redis_context == nullptr )
        {
            __LOG_ERROR__( "_redis_context = nullptr, sql=[{}]!", strsql );
            return nullptr;
        }

        auto reply = ( redisReply* )redisCommand( _redis_context, strsql.c_str() );
        if ( reply == nullptr )
        {
            if ( !IsDisconnected() )
            {
                __LOG_ERROR__( "redisreply = nullptr, [{}:{}] sql=[{}]!", _redis_context->err, _redis_context->errstr, strsql );
            }
            return nullptr;
        }

        if ( reply->type == REDIS_REPLY_ERROR )
        {
            __LOG_ERROR__( "reply error [{}:{}] sql=[{}]!", reply->type, reply->str, strsql );
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
                auto result = TryConnect();
                if ( result == KFEnum::Ok )
                {
                    redisreply = Execute( strsql );
                }
                else
                {
                    __LOG_ERROR__( "redis connect [{}:{}] failed!", _ip, _port );
                }
            }
        }

        if ( redisreply != nullptr )
        {
            kfresult->SetResult( KFEnum::Ok );
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
            __LOG_ERROR__( "redis[{}] execute error=[{}:{}]!", strsql, _redis_context->err, _redis_context->errstr );
        }

        return redisreply;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    KFWriteExecute::KFWriteExecute()
    {
        _result_queue_list.push_back( &_void_result_queue );
        _result_queue_list.push_back( &_uint64_result_queue );
    }

    KFWriteExecute::~KFWriteExecute()
    {

    }

    KFResult< voidptr >* KFWriteExecute::VoidExecute( const std::string& strsql )
    {
        auto kfresult = _void_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< uint64 >* KFWriteExecute::UpdateExecute( const std::string& strsql )
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

    // todo: 发生错误是否需要回滚
    KFResult< voidptr >* KFWriteExecute::Pipeline( const ListString& commands )
    {
        auto kfresult = _void_result_queue.Alloc();
        if ( _redis_context == nullptr )
        {
            kfresult->SetResult( KFEnum::Error );
            return kfresult;
        }

        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : commands )
        {
            redisReply* reply = nullptr;
            redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    redisGetReply( _redis_context, ( void** )&reply );
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

        return kfresult;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFReadExecute::KFReadExecute()
    {
        _result_queue_list.push_back( &_uint32_result_queue );
        _result_queue_list.push_back( &_uint64_result_queue );
        _result_queue_list.push_back( &_string_result_queue );
        _result_queue_list.push_back( &_map_result_queue );
        _result_queue_list.push_back( &_vector_result_queue );
        _result_queue_list.push_back( &_list_result_queue );
        _result_queue_list.push_back( &_list_map_result_queue );
    }

    KFReadExecute::~KFReadExecute()
    {

    }

    KFResult< std::string >* KFReadExecute::StringExecute( const std::string& strsql )
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

    KFResult< uint64 >* KFReadExecute::UInt64Execute( const std::string& strsql )
    {
        auto kfresult = _uint64_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            if ( redisreply->integer != _invalid_int )
            {
                kfresult->_value = redisreply->integer;
            }
            else if ( redisreply->str != nullptr )
            {
                kfresult->_value = KFUtility::ToValue< uint64 >( redisreply->str );
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< MapString >* KFReadExecute::MapExecute( const std::string& strsql )
    {
        auto kfresult = _map_result_queue.Alloc();
        auto redisreply = TryExecute( kfresult, strsql );
        if ( redisreply != nullptr )
        {
            for ( size_t i = 0u; i < redisreply->elements; i += 2 )
            {
                auto keyelement = redisreply->element[ i ];
                auto valueelement = redisreply->element[ i + 1 ];
                if ( keyelement->str != nullptr && valueelement->str != nullptr )
                {
                    kfresult->_value[ keyelement->str ] = valueelement->str;
                }
            }
        }

        __FREE_REPLY__( redisreply );
        return kfresult;
    }

    KFResult< VectorString >* KFReadExecute::VectorExecute( const std::string& strsql )
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

    KFResult< ListString >* KFReadExecute::ListExecute( const std::string& strsql )
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

    KFResult< ListString >* KFReadExecute::ListPipelineExecute( const ListString& commands )
    {
        auto kfresult = _list_result_queue.Alloc();
        if ( _redis_context == nullptr )
        {
            kfresult->SetResult( KFEnum::Error );
            return kfresult;
        }

        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : commands )
        {
            redisReply* reply = nullptr;
            redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    redisGetReply( _redis_context, ( void** )&reply );
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

        return kfresult;
    }

    KFResult< std::list< MapString > >* KFReadExecute::ListMapPipelineExecute( const ListString& commands )
    {
        auto kfresult = _list_map_result_queue.Alloc();
        if ( _redis_context == nullptr )
        {
            kfresult->SetResult( KFEnum::Error );
            return kfresult;
        }

        for ( auto& command : commands )
        {
            auto result = redisAppendCommand( _redis_context, command.c_str() );
            if ( result != REDIS_OK )
            {
                kfresult->SetResult( KFEnum::Error );
            }
        }

        for ( auto& command : commands )
        {
            redisReply* reply = nullptr;
            redisGetReply( _redis_context, ( void** )&reply );
            if ( reply == nullptr )
            {
                if ( IsDisconnected() )
                {
                    TryConnect();
                    redisGetReply( _redis_context, ( void** )&reply );
                }
            }

            if ( reply != nullptr )
            {
                MapString values;
                for ( size_t i = 0; i < reply->elements; i += 2 )
                {
                    auto keyelement = reply->element[ i ];
                    auto valueelement = reply->element[ i + 1 ];
                    if ( keyelement->str != nullptr && valueelement->str != nullptr )
                    {
                        values[ keyelement->str ] = valueelement->str;
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

        return kfresult;
    }
}