#ifndef __KF_GENERATE_SSH_H__
#define __KF_GENERATE_SSH_H__
#include "KFGenerateDefine.h"

namespace KFrame
{
    class KFGenerateSSH : public KFSingleton< KFGenerateSSH >
    {
    public:
        // 执行脚本
        void ExecuteCommand( const SSHData* sshdata );

    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _ssh = KFrame::KFGenerateSSH::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////
#endif