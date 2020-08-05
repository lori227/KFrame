#include "KFMessageModule.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMessageModule::AddFunction( uint32 msgid, KFMessageFunction& function )
    {
        auto kfhandle = _kf_message_handle.Find( msgid );
        if ( kfhandle == nullptr )
        {
            kfhandle = _kf_message_handle.Create( msgid );
            kfhandle->_function = function;
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] already register", msgid );
        }
    }

    bool KFMessageModule::CallFunction( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        auto kfhandle = _kf_message_handle.Find( msgid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        if ( kfhandle->_is_open )
        {
            kfhandle->_function( route, msgid, data, length );
        }
        return true;
    }

    void KFMessageModule::UnRegisterFunction( uint32 msgid )
    {
        auto ok = _kf_message_handle.Remove( msgid );
        if ( !ok )
        {
            __LOG_ERROR__( "msgid[{}] unregister failed", msgid );
        }
    }

    bool KFMessageModule::OpenFunction( uint32 msgid, bool open )
    {
        auto kfhandle = _kf_message_handle.Find( msgid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        kfhandle->_is_open = open;
        return true;
    }

}