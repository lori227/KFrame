#include "KFWorkerModule.hpp"

namespace KFrame
{
    KFWorkerModule::KFWorkerModule()
    {
        _actor_index = 0;
    }

    KFWorkerModule::~KFWorkerModule()
    {
        for ( auto kfactor : _kf_actor )
        {
            __KF_DELETE__( KFActor, kfactor );
        }
    }

    void KFWorkerModule::Run()
    {
        for ( auto kfactor : _kf_actor )
        {
            kfactor->RunActor();
        }
    }

    void KFWorkerModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_TRANSPOND__( &KFWorkerModule::SendMessageToWorker );
        //////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFWorkerModule::BeforeShut()
    {
        for ( auto kfactor : _kf_actor )
        {
            kfactor->StopActor();
        }

        __UN_TCP_SERVER_TRANSPOND__();
        //////////////////////////////////////////////////////////////////////////////////////////////
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFWorkerModule::AddFunction( uint32 msg_id, KFMessageFunction& function )
    {
        auto kffunction = _kf_message_function.Create( msg_id );
        kffunction->_function = function;
    }

    bool KFWorkerModule::CallFunction( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        auto kffunction = _kf_message_function.Find( msg_id );
        if ( kffunction == nullptr )
        {
            return false;
        }

        kffunction->_function( route, msg_id, data, length );
        return true;
    }

    void KFWorkerModule::UnRegisterFunction( uint32 msg_id )
    {
        _kf_message_function.Remove( msg_id );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFActor* KFWorkerModule::CreateWorkActor()
    {
        auto kfactor = __KF_NEW__( KFActor );
        kfactor->InitActor( this, 5000, 5000 );
        _kf_actor.push_back( kfactor );

        _actor_index = static_cast< uint32 >( _kf_actor.size() ) - 1;
        return kfactor;
    }

    KFActor* KFWorkerModule::FindWorkActor()
    {
        auto count = _kf_actor.size();
        if ( count != 0 )
        {
            auto loopcount = count;
            __DO__
            {
                --loopcount;

                _actor_index = ( _actor_index + 1 ) % count;
                auto kfactor = _kf_actor[ _actor_index ];
                if ( !kfactor->IsBusy() )
                {
                    return kfactor;
                }
            } __DO_WHILE__ ( loopcount > 0 );
        }

        return CreateWorkActor();
    }

    __KF_FORWARD_MESSAGE_FUNCTION__( KFWorkerModule::SendMessageToWorker )
    {
        auto kfactor = FindWorkActor();
        kfactor->PushReqMessage( route, msg_id, data, length );
        return true;
    }

    void KFWorkerModule::SendToClient( const Route& route, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto kftemp = const_cast< Route* >( &route );
        auto kfuint64 = reinterpret_cast< uint64* >( kftemp );
        auto kfactor = reinterpret_cast< KFActor* >( *kfuint64 );

        kfactor->PushAckMessage( route, msg_id, message );
    }

    void KFWorkerModule::SendToClient( const Route& route, uint64 server_id, uint32 msg_id, google::protobuf::Message* message )
    {
        auto kftemp = const_cast< Route* >( &route );
        auto kfuint64 = reinterpret_cast< uint64* >( kftemp );
        auto kfactor = reinterpret_cast< KFActor* >( *kfuint64 );

        kfactor->PushAckMessage( server_id, msg_id, message );
    }
}