#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFDefine.h"
#include "KFUUID.h"

namespace KFrame
{
    class KFRand;
    class KFAppId;
    class KFVersion;
    class KFMutex;

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
        uint32 Rand();
        uint32 RandRatio( uint32 ratio );

        // 范围内随机
        uint32 RandRange( uint32 min, uint32 max, uint32 base );

        // 浮点随机
        double RandDouble();
        double RandDouble( double first, double second );

        // 创建uuid
        uint64 STMakeUUID();
        uint64 MTMakeUUID();
        uint64 STMakeUUID( const std::string& name );
        uint64 MTMakeUUID( const std::string& name );

        // 获得zoneid
        uint32 STUUIDZoneId( const std::string& name, uint64 uuid );
        uint32 MTUUIDZoneId( const std::string& name, uint64 uuid );

        // 判断游戏分区id
        bool IsServerSameZone( uint64 serverid );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 初始化网络
        void InitNetType( std::string& strtype );

        // 渠道类型
        void InitChannelService( std::string& strtype );

        // 判断渠道和服务类型
        bool CheckChannelService( uint32 channel, uint32 service );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 设置uuid
        void UUIDSetting( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits, uint64 starttime );

    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

        // 创建uuid
        KFUUID* CreateUUID( const std::string& name );

    public:
        // 程序运行
        std::atomic<bool> _app_run;

        // 现实时间( 单位 : 秒 )
        uint64 _real_time;

        // 游戏时间( 单位 : 毫秒 )
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
        StringMap _startup_params;

        /////////////////////////////////////////////////////////////////////////////////
    private:
        // 版本
        KFVersion* _kf_version;

        // 单线程随机类
        KFRand* _kf_rand;

        // uuid
        KFMutex* _kf_mutex = nullptr;
        KFUUID* _kf_uuid = nullptr;
        std::unordered_map< std::string, KFUUID*> _kf_uuids;
    };
    //////////////////////////////////////////////////////////////////////////////////////////

}

#endif