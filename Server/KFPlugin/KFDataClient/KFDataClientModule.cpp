#include "KFDataClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDataClientModule::KFDataClientModule()
    {
    }

    KFDataClientModule::~KFDataClientModule()
    {

    }

    void KFDataClientModule::InitModule()
    {

    }

    void KFDataClientModule::BeforeRun()
    {
    }

    void KFDataClientModule::BeforeShut()
    {
    }

    bool KFDataClientModule::SendMessageToData( uint32 zoneid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2STransmitToDataShardReq req;
        req.set_zoneid( zoneid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster->SendMessageToShard( KFField::_data, KFMsg::S2S_TRANSMIT_TO_DATA_SHARD_REQ, &req );
    }
}
