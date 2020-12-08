#include "KFMessageModule.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMessageModule::AddMessageHandle( KFMessageHandleAbstract* messagehandle )
    {
        auto ok = RemoveMessageHandle( messagehandle->_msgid, messagehandle->GetModule() );
        if ( ok )
        {
            __LOG_ERROR__( "msgid=[{}] already register", messagehandle->_msgid );
        }

        _handles[ messagehandle->_msgid ] = messagehandle;
    }

    bool KFMessageModule::RemoveMessageHandle( uint32 msgid, KFModule* module )
    {
        auto iter = _handles.find( msgid );
        if ( iter == _handles.end() )
        {
            return false;
        }

        __KF_DELETE__( KFMessageHandleAbstract, iter->second );
        _handles.erase( iter );
        return true;
    }

    bool KFMessageModule::HandleMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        auto iter = _handles.find( msgid );
        if ( iter == _handles.end() )
        {
            return false;
        }

        iter->second->CallFunction( route, data, length );
        return true;
    }

    bool KFMessageModule::OpenFunction( uint32 msgid, bool open )
    {
        auto iter = _handles.find( msgid );
        if ( iter == _handles.end() )
        {
            return false;
        }

        iter->second->OpenFunction( open );
        return true;
    }

    void KFMessageModule::BindFindEngityFunction( KFFindEntityFunction& function )
    {
        _find_entity_function = function;
    }

    void KFMessageModule::UnBindFindEngityFunction()
    {
        _find_entity_function = nullptr;
    }

}