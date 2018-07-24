#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFLoggerEx;

    class KFGlobal
    {
    public:
        ~KFGlobal();

        // 创建
        static void Initialize( KFGlobal* kfglobal );

        // 接口
        static KFGlobal* Instance();

        // 获得程序标题
        std::string FormatTitleText( const std::string& appname, const std::string& apptype, uint32 appid );

    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

    public:
        // 程序运行
        volatile bool _app_run;

        // 现实时间
        uint64 _real_time;

        // 游戏时间
        uint64 _game_time;

        // appid
        uint32 _app_id;

        // 类型
        std::string _app_type;

        // 名字
        std::string _app_name;

        // 分区id
        uint32 _zone_id;

        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interanet_ip;

        // 监听端口
        uint32 _listen_port;

        // 程序标题
        std::string _title_text;

        KFLoggerEx* _logger;
    };
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
}

#endif