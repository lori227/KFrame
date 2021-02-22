#include "KFMessageModule.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMessageModule::AddMessageHandle( KFMessageHandleAbstract* messagehandle )
    {
        auto ok = RemoveMessageHandle( messagehandle->_msg_id, messagehandle->GetModule() );
        if ( ok )
        {
            __LOG_ERROR__( "msg_id=[{}] already register", messagehandle->_msg_id );
        }

        _handles[ messagehandle->_msg_id ] = messagehandle;
    }

    bool KFMessageModule::RemoveMessageHandle( uint32 msg_id, KFModule* module )
    {
        auto iter = _handles.find( msg_id );
        if ( iter == _handles.end() )
        {
            return false;
        }

        __KF_DELETE__( KFMessageHandleAbstract, iter->second );
        _handles.erase( iter );
        return true;
    }

    bool KFMessageModule::HandleMessage( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        auto iter = _handles.find( msg_id );
        if ( iter == _handles.end() )
        {
            return false;
        }

        KFEntity* kfentity = nullptr;
        auto kfhandle = iter->second;
        auto findentityfunction = _find_entity_function.Find( kfhandle->_type );
        if ( findentityfunction != nullptr )
        {
            kfentity = findentityfunction->_function( route._recv_id );
            if ( kfentity == nullptr )
            {
                __LOG_WARN__( "msg_id=[{}:{}] can't find entity=[{}]!", kfhandle->_type, kfhandle->_message->GetTypeName(), route._recv_id );
                return true;
            }
        }

        kfhandle->CallFunction( kfentity, route, data, length );
        return true;
    }

    bool KFMessageModule::OpenFunction( uint32 msg_id, bool open )
    {
        auto iter = _handles.find( msg_id );
        if ( iter == _handles.end() )
        {
            return false;
        }

        iter->second->OpenFunction( open );
        return true;
    }

    void KFMessageModule::BindFindEntityFunction( uint32 type, KFModule* module, KFFindEntityFunction& function )
    {
        auto kffunction = _find_entity_function.Create( type );
        kffunction->SetFunction( module, function );
    }

    void KFMessageModule::UnBindFindEntityFunction( uint32 type )
    {
        _find_entity_function.Remove( type );
    }

}