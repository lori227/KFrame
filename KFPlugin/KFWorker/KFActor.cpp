#include "KFActor.hpp"
#include "KFWorkerModule.hpp"
#include "google/protobuf/message.h"

namespace KFrame
{
    KFActor::KFActor()
    {
        _actor_thread_run = false;
        _kf_worker_moduler = nullptr;
    }

    KFActor::~KFActor()
    {

    }

    void KFActor::InitActor( KFWorkerModule* kfmodule, uint32 maxreqcount, uint32 maxackcount )
    {
        _kf_worker_moduler = kfmodule;
        _req_message_queue.InitQueue( maxreqcount, maxreqcount );
        _ack_message_queue.InitQueue( maxackcount, maxreqcount );

        // 创建执行线程
        _actor_thread_run = true;
        KFThread::CreateThread( this, &KFActor::RunActorThread, __FUNC_LINE__ );
    }

    void KFActor::StopActor()
    {
        _actor_thread_run = false;
    }

    bool KFActor::IsBusy()
    {
        return _req_message_queue.IsFull();
    }

    void KFActor::RunActor()
    {
        // 为了给其他模块腾出cpu时间, 这里每次最多处理100个
        static const uint32 _max_message_count = 100;
        uint32 count = 0;

        do
        {
            auto message = _ack_message_queue.PopObject();
            if ( message == nullptr )
            {
                break;
            }

            //_kf_cluster_shard->SendToClient( message->_kfid, message->_msgid, message->_data, message->_length );
            __KF_DELETE__( KFWorkerMessage, message );
        } while ( ++count < _max_message_count );
    }


    void KFActor::RunActorThread()
    {
        while ( _actor_thread_run )
        {
            auto message = _req_message_queue.PopObject();
            if ( message == nullptr )
            {
                KFThread::Sleep( 1 );
            }
            else
            {
                _route = message->_route;

                auto* kfactor = reinterpret_cast< uint64* >( &message->_route );
                *kfactor = reinterpret_cast< uint64 >( this );

                _kf_worker_moduler->CallFunction( message->_route, message->_msgid, message->_data, message->_length );
                __KF_DELETE__( KFWorkerMessage, message );
            }
        }
    }

    bool KFActor::PushReqMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        if ( _req_message_queue.IsFull() )
        {
            return false;
        }

        auto message = __KF_NEW__( KFWorkerMessage );
        message->CopyFrom( route, msgid, data, length );
        _req_message_queue.PushObject( message );
        return true;
    }

    bool KFActor::PushAckMessage( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _ack_message_queue.IsFull() )
        {
            return false;
        }

        auto strdata = message->SerializeAsString();

        auto workermessage = __KF_NEW__( KFWorkerMessage );
        workermessage->CopyFrom( _route, msgid, strdata.data(), static_cast< uint32 >( strdata.size() ) );
        _ack_message_queue.PushObject( workermessage );
        return true;
    }


    bool KFActor::PushAckMessage( uint64 serverid, uint32 msgid, google::protobuf::Message* message )
    {
        if ( _ack_message_queue.IsFull() )
        {
            return false;
        }

        Route route( 0, 0, serverid );
        auto strdata = message->SerializeAsString();

        auto workermessage = __KF_NEW__( KFWorkerMessage );
        workermessage->CopyFrom( route, msgid, strdata.data(), static_cast< uint32 >( strdata.size() ) );
        _ack_message_queue.PushObject( workermessage );
        return true;
    }
}