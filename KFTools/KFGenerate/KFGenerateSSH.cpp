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
            kfssh.Execute( sshdata->_command_list );
        }

        KFSSH::Shutdown();
    }
}