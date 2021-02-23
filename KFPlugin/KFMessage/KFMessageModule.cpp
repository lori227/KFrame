#include "KFMessageModule.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMessageModule::AddMessageHandle( std::shared_ptr<KFMessageHandleAbstract> message_handle )
    {
        auto ok = RemoveMessageHandle( message_handle->_msg_id, message_handle->GetModule() );
        if ( ok )
        {
            __LOG_ERROR__( "msg_id=[{}] already register", message_handle->_msg_id );
        }

        _handle_list.Insert( message_handle->_msg_id, message_handle );
    }

    bool KFMessageModule::RemoveMessageHandle( uint32 msg_id, KFModule* module )
    {
        return _handle_list.Remove( msg_id ) != nullptr;
    }

    bool KFMessageModule::HandleMessage( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        auto message_handle = _handle_list.Find( msg_id );
        if ( message_handle == nullptr )
        {
            return false;
        }

        EntityPtr entity = nullptr;
        auto find_entity_function = _find_entity_function.Find( message_handle->_type );
        if ( find_entity_function != nullptr )
        {
            entity = find_entity_function->CallEx<EntityPtr>( route._recv_id );
            if ( entity == nullptr )
            {
                __LOG_WARN__( "msg_id=[{}:{}] can't find entity=[{}]!", message_handle->_type, message_handle->_message->GetTypeName(), route._recv_id );
                return true;
            }
        }

        message_handle->CallFunction( entity, route, data, length );
        return true;
    }

    bool KFMessageModule::OpenFunction( uint32 msg_id, bool open )
    {
        auto message_handle = _handle_list.Find( msg_id );
        if ( message_handle == nullptr )
        {
            return false;
        }

        message_handle->OpenFunction( open );
        return true;
    }

    void KFMessageModule::BindFindEntityFunction( uint32 type, KFModule* module, KFFindEntityFunction& function )
    {
        auto module_function = _find_entity_function.Create( type );
        module_function->SetFunction( module, function );
    }

    void KFMessageModule::UnBindFindEntityFunction( uint32 type )
    {
        _find_entity_function.Remove( type );
    }

}