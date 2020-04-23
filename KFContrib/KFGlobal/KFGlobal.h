#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFDefine.h"
#include "KFConstant.h"

namespace KFrame
{
    class KFRand;
    class KFAppId;
    class KFVersion;
    class KFUuid;
    class KFConstantData;

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
        uint64 RandRange( uint64 min, uint64 max, uint64 base = 1u );

        // 浮点随机
        double RandDouble();
        double RandDouble( double first, double second );

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
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化配置
        void AddUuidSetting( const std::string& name, uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits );

        // 生成uuid
        uint64 STMakeUuid();
        uint64 MTMakeUuid();

        uint64 STMakeUuid( const std::string& name );
        uint64 MTMakeUuid( const std::string& name );

        uint64 STMakeUuid( const std::string& name, uint32 zoneid );
        uint64 MTMakeUuid( const std::string& name, uint32 zoneid );

        // 获得zoneid
        uint32 STUuidZoneId( const std::string& name, uint64 uuid );
        uint32 MTUuidZoneId( const std::string& name, uint64 uuid );

        // 解析guid( time, zone, worker, seq )
        std::tuple<uint64, uint32, uint32, uint32> STUuidParse( const std::string& name, uint64 uuid );
        std::tuple<uint64, uint32, uint32, uint32> MTUuidParse( const std::string& name, uint64 uuid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加常量
        void AddConstant( const std::string& name, uint32 key, uint32 value );
        void AddConstant( const std::string& name, uint32 key, double value );
        void AddConstant( const std::string& name, uint32 key, const std::string& value );

        // 获得常量
        const KFConstant* FindConstant( const std::string& name, uint32 key = 0u );

        // uint32配置
        uint32 GetUInt32( const std::string& name, uint32 key = 0u );

        // double配置
        double GetDouble( const std::string& name, uint32 key = 0u );

        // string配置
        const std::string& GetString( const std::string& name, uint32 key = 0u );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

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
        ////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 项目开始时间
        uint64 _project_time = 0u;
        ////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 版本
        KFVersion* _kf_version = nullptr;

        // 单线程随机类
        KFRand* _kf_rand = nullptr;

        // uuid
        KFUuid* _kf_uuid = nullptr;

        // 常量配置
        KFConstantData* _kf_constant_data = nullptr;
    };
    //////////////////////////////////////////////////////////////////////////////////////////

}

#endif