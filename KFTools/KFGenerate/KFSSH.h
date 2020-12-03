#ifndef __KF_SSH_H__
#define __KF_SSH_H__

#include "KFGenerateDefine.h"
#include "libssh2.h"

namespace KFrame
{
    class KFSSH
    {
    public:
        static void Init();

        // 关闭资源
        static void Shutdown();

    public:
        KFSSH( const std::string& ip, uint32 port, const std::string& user, const std::string& password );
        ~KFSSH();

        // command 被远程执行的命令，如：whoami
        bool Execute( const std::string& command );

    protected:
        int32 GetErrorCode() const;

        // 获得错误信息
        std::string GetErrorMessage() const;

        // 清除资源
        void Cleanup();

        // 创建连接
        bool CreateSession( bool nonblocking );

        // 握手
        void HandShake();

        // 等待超时
        bool TimeWaitSocket();

        // 设置host
        void SetKnownHosts();

        // 验证密码
        void ValidateAuthorization( const std::string& password );

        // 打开ssh channel
        LIBSSH2_CHANNEL* OpenChannel();

        // 关闭 ssh channel
        int32 CloseChannel( LIBSSH2_CHANNEL* channel, std::string* exitsignal, std::string* errmsg );

        // 读取 ssh channel
        int32 ReadChannel( LIBSSH2_CHANNEL* channel );
    private:
        int32 _socket_fd = 0;
        LIBSSH2_SESSION* _session = nullptr;
        std::string _ip;
        uint32 _port = 0u;
        std::string _user;
    };
}

#endif
