#include "KFSSH.h"
#include "KFGenerateEvent.h"

namespace KFrame
{
    void KFSSH::Init()
    {
        libssh2_init( LIBSSH2_INIT_NO_CRYPTO );
    }

    void KFSSH::Shutdown()
    {
        libssh2_exit();
    }

    KFSSH::KFSSH( const std::string& ip, uint32 port, const std::string& user, const std::string& password )
    {
        _ip = ip;
        _port = port;
        _user = user;
        auto ok = CreateSession( false );
        if ( ok )
        {
            try
            {
                HandShake();
                //SetKnownHosts();
                ValidateAuthorization( password );
            }
            catch ( ... )
            {
                Cleanup();
            }
        }
    }

    KFSSH::~KFSSH()
    {
        Cleanup();
    }

    void KFSSH::Cleanup()
    {
        if ( _session != nullptr )
        {
            libssh2_session_disconnect( _session, "normal Shutdown" );
            libssh2_session_free( _session );
            _session = nullptr;
        }

        if ( _socket_fd != -1 )
        {
            closesocket( _socket_fd );
            _socket_fd = -1;
        }

        WSACleanup();
    }

    int32 KFSSH::GetErrorCode() const
    {
        if ( _session == nullptr )
        {
            return 0;
        }

        return libssh2_session_last_errno( _session );
    }

    std::string KFSSH::GetErrorMessage() const
    {
        if ( _session == nullptr )
        {
            return _invalid_string;
        }

        std::string result;
        char* errmsg;
        int length = 0;
        libssh2_session_last_error( _session, &errmsg, &length, 1 );
        if ( errmsg != nullptr )
        {
            result = errmsg;
            free( errmsg );
        }

        return result;
    }

    bool KFSSH::CreateSession( bool nonblocking )
    {
        WSADATA wsadata;
        auto versionrequested = MAKEWORD( 1, 1 );
        WSAStartup( versionrequested, &wsadata );

        _socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
        if ( -1 == _socket_fd )
        {
            _event->AddEvent( EventType::SSHFailed, GetLastError(), "初始化socket失败" );
            return false;
        }

        try
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons( _port );
            addr.sin_addr.s_addr = inet_addr( _ip.c_str() );

            if ( -1 == connect( _socket_fd, ( struct sockaddr* )&addr, sizeof( struct sockaddr_in ) ) )
            {
                _event->AddEvent( EventType::SSHFailed, GetLastError(), "连接ssh服务器失败" );
                return false;
            }

            // 创建ssh2会话
            _session = libssh2_session_init();

            //if ( nonblocking ) // 设置为非阻塞
            //{
            //   libssh2_session_set_blocking( _session, 0 );
            //}
            //else
            //{
            libssh2_session_set_timeout( _session, 30000000 );
            //}

            return true;
        }
        catch ( ... )
        {
            Cleanup();
        }

        return false;
    }

    void KFSSH::HandShake()
    {
        do
        {
            // ssh2握手
            int errcode = libssh2_session_handshake( _session, _socket_fd );
            if ( 0 == errcode )
            {
                break;
            }

            if ( !TimeWaitSocket() )
            {
                break;
            }

        } while ( true );
    }

    bool KFSSH::TimeWaitSocket()
    {
        return false;
    }

    void KFSSH::SetKnownHosts()
    {
        int type;
        size_t len;
        struct libssh2_knownhost* known_host;


        LIBSSH2_KNOWNHOSTS* knownhosts = libssh2_knownhost_init( _session );
        if ( knownhosts == nullptr )
        {
            _event->AddEvent( EventType::SSHFailed, GetErrorCode(), GetErrorMessage() );
            return;
        }

        try
        {
            // read all hosts from here
            libssh2_knownhost_readfile( knownhosts, "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH );
            // store all known hosts to here
            libssh2_knownhost_writefile( knownhosts, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH );

            auto fingerprint = libssh2_session_hostkey( _session, &len, &type );
            if ( fingerprint == nullptr )
            {
                _event->AddEvent( EventType::SSHFailed, GetErrorCode(), GetErrorMessage() );
                return;
            }

            // introduced in 1.2.6
            auto check = libssh2_knownhost_checkp(
                             knownhosts, _ip.c_str(), _port,
                             fingerprint, len, LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW, &known_host );

            libssh2_knownhost_free( knownhosts );
        }
        catch ( ... )
        {
            libssh2_knownhost_free( knownhosts );
        }
    }

    void KFSSH::ValidateAuthorization( const std::string& password )
    {
        auto errcode = libssh2_userauth_password( _session, _user.c_str(), password.c_str() );
        if ( errcode != 0 )
        {
            _event->AddEvent( EventType::SSHFailed, GetErrorCode(), GetErrorMessage() );
        }
    }

    LIBSSH2_CHANNEL* KFSSH::OpenChannel()
    {
        // 建立会话通道
        auto channel = libssh2_channel_open_session( _session );
        return channel;
    }

    int32 KFSSH::CloseChannel( LIBSSH2_CHANNEL* channel, std::string* exitsignal, std::string* errmsg )
    {
        auto exitcode = 127; // 127: command not exists

        auto errcode = libssh2_channel_close( channel );
        if ( 0 == errcode )
        {
            char* errmsg_ = nullptr;
            char* exitsignal_ = nullptr;

            exitcode = libssh2_channel_get_exit_status( channel );
            if ( exitcode != 0 ) // 0 success
            {
                // 调用端可以strerror(*exitcode)取得出错原因
                libssh2_channel_get_exit_signal( channel, &exitsignal_, nullptr, &errmsg_, nullptr, nullptr, nullptr );
                if ( errmsg_ != nullptr )
                {
                    *errmsg = errmsg_;
                    free( errmsg_ );
                }
                if ( exitsignal_ != nullptr )
                {
                    *exitsignal = exitsignal_;
                    free( exitsignal_ );
                }
            }

            libssh2_channel_free( channel );
        }

        return exitcode;
    }

    int32 KFSSH::ReadChannel( LIBSSH2_CHANNEL* channel )
    {
        int numbytes = 0;
        do
        {
            char buffer[ 4096 ];
            auto bytes = libssh2_channel_read( channel, buffer, sizeof( buffer ) - 1 );
            if ( bytes == 0 )
            {
                break;
            }

            if ( bytes > 0 )
            {
                numbytes += bytes;
                buffer[ bytes ] = '\0';

            }
        } while ( true );

        return numbytes;
    }

    bool KFSSH::Execute( const StringList& commandlist )
    {
        auto channel = OpenChannel();
        if ( channel == nullptr )
        {
            _event->AddEvent( EventType::SSHFailed, GetErrorCode(), GetErrorMessage() );
            return false;
        }

        int32 numbytes = 0;
        try
        {
            for ( auto& command : commandlist )
            {
                do
                {
                    int errcode = libssh2_channel_exec( channel, command.c_str() );
                    if ( 0 == errcode )
                    {
                        break;
                    }

                    KFThread::Sleep( 10 );
                } while ( true );

                numbytes = ReadChannel( channel );
            }
        }
        catch ( ... )
        {
        }

        std::string errmsg;
        std::string exitsignal;
        auto exitcode = CloseChannel( channel, &exitsignal, &errmsg );

        return true;
    }

}