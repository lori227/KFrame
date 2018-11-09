#include "KFActor.h"
#include "KFWorkerModule.h"
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
        _req_message_queue.InitQueue( maxreqcount );
        _ack_message_queue.InitQueue( maxackcount );

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

            _kf_cluster_shard->SendToClient( message->_kfguid, message->_msgid, message->_data, message->_length );
            __KF_DESTROY__( KFWorkerMessage, message );
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
                _kf_guid = message->_kfguid;

                auto* kfactor = reinterpret_cast< uint64* >( &message->_kfguid );
                *kfactor = reinterpret_cast< uint64 >( this );

                _kf_worker_moduler->CallFunction( message->_kfguid, message->_msgid, message->_data, message->_length );
                __KF_DESTROY__( KFWorkerMessage, message );
            }
        }
    }

    bool KFActor::PushReqMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        if ( _req_message_queue.IsFull() )
        {
            return false;
        }

        auto message = __KF_CREATE_BATCH__( KFWorkerMessage, 100 );
        message->CopyFrom( kfguid, msgid, data, length );
        _req_message_queue.PushObject( message );
        return true;
    }

    bool KFActor::PushAckMessage( const KFGuid& kfguid, uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _ack_message_queue.IsFull() )
        {
            return false;
        }

        auto strdata = message->SerializeAsString();

        auto workermessage = __KF_CREATE_BATCH__( KFWorkerMessage, 100 );
        workermessage->CopyFrom( _kf_guid, msgid, strdata.data(), static_cast< uint32 >( strdata.size() ) );
        _ack_message_queue.PushObject( workermessage );
        return true;
    }


    bool KFActor::PushAckMessage( uint32 serverid, uint32 msgid, google::protobuf::Message* message )
    {
        if ( _ack_message_queue.IsFull() )
        {
            return false;
        }

        KFGuid kfguid;
        kfguid._head_id = _invalid_int;
        kfguid._data_id = serverid;

        auto strdata = message->SerializeAsString();

        auto workermessage = __KF_CREATE_BATCH__( KFWorkerMessage, 100 );
        workermessage->CopyFrom( kfguid, msgid, strdata.data(), static_cast< uint32 >( strdata.size() ) );
        _ack_message_queue.PushObject( workermessage );
        return true;
    }
}