#include "KFLogClientModule.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    KFLogClientModule::KFLogClientModule()
    {
    }

    KFLogClientModule::~KFLogClientModule()
    {

    }

    void KFLogClientModule::InitModule()
    {

    }

    void KFLogClientModule::BeforeRun()
    {

    }

    void KFLogClientModule::BeforeShut()
    {

    }

    void KFLogClientModule::LogRemote( const int log_level, const int log_category, const std::string& log_info )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SLogReq req;
        req.set_log_level( log_level );
        req.set_log_category( log_category );
        req.set_app_name( kfglobal->_app_name );
        req.set_app_type( kfglobal->_app_type );
        req.set_app_id( kfglobal->_app_id );
        req.set_zone_id( kfglobal->_zone_id );
        req.set_log_info( log_info );

        SendMessageToLog( KFMsg::S2S_LOG_REQ, &req );
    }

    bool KFLogClientModule::SendMessageToLog( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( log ), msgid, message );
    }

}

