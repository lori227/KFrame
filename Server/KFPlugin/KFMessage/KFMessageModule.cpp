#include "KFMessageModule.h"

namespace KFrame
{
    KFMessageModule::KFMessageModule()
    {

    }

    KFMessageModule::~KFMessageModule()
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMessageModule::AddFunction( uint32 msgid, KFMessageFunction& function )
    {
        auto kffunction = _kf_message_function.Find( msgid );
        if ( kffunction != nullptr )
        {
            __LOG_ERROR__( KFLogEnum::System, "msgid[{}] already register!", msgid );
            return;
        }

        kffunction = _kf_message_function.Create( msgid );
        kffunction->_function = function;
    }

    bool KFMessageModule::CallFunction( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )
    {
        auto kffunction = _kf_message_function.Find( msgid );
        if ( kffunction == nullptr )
        {
            return false;
        }

        kffunction->_function( guid, data, length );
        return true;
    }

    void KFMessageModule::UnRegisterFunction( uint32 msgid )
    {
        auto ok = _kf_message_function.Remove( msgid );
        if ( !ok )
        {
            __LOG_ERROR__( KFLogEnum::System, "msgid[{}] unregister failed!", msgid );
        }
    }

}