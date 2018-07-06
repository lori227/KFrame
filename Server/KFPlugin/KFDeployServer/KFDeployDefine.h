#ifndef __KF_DEPLOY_DEFINE_H__
#define __KF_DEPLOY_DEFINE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFServerData
    {
    public:
        KFServerData()
        {
            _app_id = 0;
            _process_id = 0;
            _startup_time = 0;
            _is_shutdown = false;
        }

    public:
        // 服务器id
        uint32 _app_id;

        // 服务器名字
        std::string _app_name;

        // 服务器类型
        std::string _app_type;

        // 进程id
        uint32 _process_id;

        // 启动时间
        uint64 _startup_time;

        // 是否已经关闭
        bool _is_shutdown;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFAgentData
    {
    public:
        KFAgentData()
        {
            _agent_id = 0;
            _port = 0;
        }

    public:
        // 服务器id
        uint32 _agent_id;

        // 局域网地址
        std::string _local_ip;

        // 名字
        std::string _name;

        // 类型
        std::string _type;

        // 端口
        uint32 _port;

        // 服务器列表
        KFMap< uint32, uint32, KFServerData > _server_list;
    };
}

#endif