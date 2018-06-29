#ifndef __KF_CLIENT_CONFIG_H__
#define __KF_CLIENT_CONFIG_H__

#include "KFrame.h"
#include "KFSingleton.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFTcpClientConfig : public KFConfig, public KFSingleton< KFTcpClientConfig >
    {
    public:

        // 加载配置文件
        bool LoadConfig( const char* file );

        // 是否需要连接
        bool NeedToConnection( const std::string& name, const std::string& type, uint32 id ) const;

    public:
        // 最大消息队列
        uint32 _max_queue_size;

        // master连接
        KFConnection _kf_master_connection;

        // 连接列表
        std::vector< KFConnection > _kf_connections;
    };

}

////////////////////////////////////////////////////////////////////////////
static auto _kf_client_config = KFrame::KFTcpClientConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
