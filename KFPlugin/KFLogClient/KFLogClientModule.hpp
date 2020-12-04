#ifndef __KF_LOG_CLIENT_MODULE__
#define __KF_LOG_CLIENT_MODULE__

#include "KFLogClientInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFLogData
    {
    public:
        uint32 _level = 0u;
        std::string _content;
    };

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class KFLogClientModule : public KFLogClientInterface
    {
    public :
        KFLogClientModule() = default;
        ~KFLogClientModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        virtual void BeforeShut();

        // run
        virtual void Run();

        // 远程日志
        void LogRemote( uint32 level, const std::string& content );

    protected:
        // 连接日志服务器成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectLogServer );

    private:
        // log服务器id
        uint64 _log_server_id = 0;

        // 线程锁
        KFMutex _kf_mutex;

        // 缓存的log队列
        std::list< KFLogData* > _log_data_list;
    };
}


#endif //!__KF_LOG_CLIENT_MODULE__