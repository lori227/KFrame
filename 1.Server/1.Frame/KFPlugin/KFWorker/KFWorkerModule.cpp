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
        __REGISTER_SERVER_TRANSMIT_FUNCTION__( &KFWorkerModule::SendMessageToWorker );
        //////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFWorkerModule::BeforeShut()
    {
        for ( auto kfactor : _kf_actor )
        {
            kfactor->StopActor();
        }

        __UNREGISTER_SERVER_TRANSMIT_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFWorkerModule::AddFunction( uint32 msgid, KFMessageFunction& function )
    {
        auto kffunction = _kf_message_function.Create( msgid );
        kffunction->_function = function;
    }

    bool KFWorkerModule::CallFunction( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        auto kffunction = _kf_message_function.Find( msgid );
        if ( kffunction == nullptr )
        {
            return false;
        }

        kffunction->_function( route, data, length );
        return true;
    }

    void KFWorkerModule::UnRegisterFunction( uint32 msgid )
    {
        _kf_message_function.Remove( msgid );
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
            do
            {
                --loopcount;

                _actor_index = ( _actor_index + 1 ) % count;
                auto kfactor = _kf_actor[ _actor_index ];
                if ( !kfactor->IsBusy() )
                {
                    return kfactor;
                }
            } while ( loopcount > 0 );
        }

        return CreateWorkActor();
    }

    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFWorkerModule::SendMessageToWorker )
    {
        auto kfactor = FindWorkActor();
        kfactor->PushReqMessage( route, msgid, data, length );
        return true;
    }

    void KFWorkerModule::SendToClient( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kftemp = const_cast< Route* >( &route );
        auto kfuint64 = reinterpret_cast< uint64* >( kftemp );
        auto kfactor = reinterpret_cast< KFActor* >( *kfuint64 );

        kfactor->PushAckMessage( route, msgid, message );
    }

    void KFWorkerModule::SendToClient( const Route& route, uint64 serverid, uint32 msgid, google::protobuf::Message* message )
    {
        auto kftemp = const_cast< Route* >( &route );
        auto kfuint64 = reinterpret_cast< uint64* >( kftemp );
        auto kfactor = reinterpret_cast< KFActor* >( *kfuint64 );

        kfactor->PushAckMessage( serverid, msgid, message );
    }
}