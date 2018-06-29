#include "KFGuildClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFGuildClientModule::KFGuildClientModule()
    {
    }

    KFGuildClientModule::~KFGuildClientModule()
    {

    }

    void KFGuildClientModule::InitModule()
    {
    }

    void KFGuildClientModule::BeforeRun()
    {
    }

    void KFGuildClientModule::BeforeShut()
    {
    }

    void KFGuildClientModule::OnceRun()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGuildClientModule::SendMessageToGuild( uint64 guildid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToObject( KFField::_guild, guildid, msgid, message );
    }
}
