#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFRand;
    class KFUUID;
    class KFAppId;
    class KFVersion;

    class KFGlobal
    {
    public:
        ~KFGlobal();

        // 创建
        static void Initialize( KFGlobal* kfglobal );

        // 接口
        static KFGlobal* Instance();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 加载版本
        bool LoadVersion( const std::string& file );

        // 判断版本
        bool CheckVersionCompatibility( const std::string& version );

        // 版本号
        const std::string& GetVersion();

        // 随机数
        uint32 RandRatio( uint32 ratio );

        // 范围内随机
        uint32 RandRange( uint32 min, uint32 max, uint32 base );

        // 浮点随机
        double RandDouble();
        double RandDouble( double first, double second );

        // 创建guid
        uint64 MakeUUID();
        uint64 MakeUUID( uint32 type );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化网络
        void InitNetType( std::string& strtype );

        // 渠道类型
        void InitChannelService( std::string& strtype );

        // 判断渠道和服务类型
        bool CheckChannelService( uint32 channel, uint32 service );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

    public:
        // 程序运行
        std::atomic<bool> _app_run;

        // 现实时间
        uint64 _real_time;

        // 游戏时间
        uint64 _game_time;

        // appid
        KFAppId* _app_id;

        // 类型
        std::string _app_type;

        // 名字
        std::string _app_name;

        // 网络类型( 内外网)
        uint32 _net_type;

        // 渠道id
        uint32 _channel;

        // 运行类型( 正式 测试 )
        uint32 _service;

        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interanet_ip;

        // 监听端口
        uint32 _listen_port;

        // 程序标题
        std::string _title_text;

        // 启动参数
        MapString _startup_params;
        /////////////////////////////////////////////////////////////////////////////////
    private:
        // 版本
        KFVersion* _kf_version;

        // 单线程随机类
        KFRand* _kf_rand;

        // guid
        std::unordered_map< uint32, KFUUID*> _kf_uuids;
    };
    //////////////////////////////////////////////////////////////////////////////////////////

}

#endif