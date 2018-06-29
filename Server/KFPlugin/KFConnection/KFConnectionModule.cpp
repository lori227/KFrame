#include "KFConnectionModule.h"
#include "KFConnectionConfig.h"

namespace KFrame
{
    KFConnectionModule::KFConnectionModule()
    {
    }

    KFConnectionModule::~KFConnectionModule()
    {

    }

    void KFConnectionModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_connection_config, true );
    }


    void KFConnectionModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFConnectionModule::GetListenPort() const
    {
        return _kf_connection_config->_listen_port;
    }

    const std::string& KFConnectionModule::FindPlatformAddress( uint32 id )
    {
        return _kf_connection_config->FindPlatformAddress( id );
    }

    const KFConnection* KFConnectionModule::FindMasterConnection( const std::string& name )
    {
        return _kf_connection_config->FindMasterConnection( name );
    }

    void KFConnectionModule::SetMasterConnection( const std::string& name, uint32 zoneid )
    {
        _kf_connection_config->SetMasterConnection( name, zoneid );
    }
}
