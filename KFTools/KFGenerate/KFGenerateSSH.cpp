#include "KFGenerateSSH.h"
#include "KFSSH.h"
#include "KFGenerateEvent.h"

namespace KFrame
{
    void KFGenerateSSH::ExecuteCommand( const SSHData* sshdata )
    {
        KFSSH::Init();

        {
            KFSSH kfssh( sshdata->_ip, sshdata->_port, sshdata->_user, sshdata->_password );

            for ( auto& command : sshdata->_command_list )
            {
                auto ok = kfssh.Execute( command );
                if ( ok )
                {
                    _event->AddEvent( EventType::SSHOk, 0, command );
                }
            }
        }

        KFSSH::Shutdown();
    }
}