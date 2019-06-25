#ifndef __KF_MONGO_CONFIG_H__
#define __KF_MONGO_CONFIG_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMongoSetting
    {
    public:
        // 逻辑id
        uint32 _id = 0;

        //ip
        std::string _ip;

        // 端口
        uint32 _port = 0;

        // 用户名
        std::string _user;

        // 密码
        std::string _password;

        // 数据库名
        std::string _database;

        // 连接超时时间
        uint32 _connect_timeout = 10000;

        // 执行超时时间
        uint32 _execute_timeout = 10000;

        // 是否使用ssl
        std::string _use_ssl;

        // 认证方式
        std::string _auth_type;
    };

    class KFMongoList
    {
    public:
        // 清空
        void Reset();

        const KFMongoSetting* FindSetting();
        void AddSetting( KFMongoSetting& kfsetting );

    private:
        std::vector< KFMongoSetting > _mongo_list;

        // 返回的连接设置
        KFMongoSetting* _kf_seting;
    };


    class KFMongoType
    {
    public:
        // 查询list
        KFMongoList* FindMongoList( uint32 type );

        // 添加list
        KFMongoList* AddMongoList( uint32 type );

    public:
        uint32 _id;

        // 列表
        KFHashMap< uint32, uint32, KFMongoList > _mongo_list;
    };

    class KFMongoConfig : public KFConfig, public KFSingleton< KFMongoConfig >
    {
    public:
        KFMongoConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        // 加载配置文件
        void LoadConfig( const std::string& file );

        // 查找配置
        KFMongoType* FindMongoType( const std::string& module, uint32 logicid );

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFMongoType > _mongo_type;
    };

    ////////////////////////////////////////////////////////////////////////////
    static auto* _kf_mongo_config = KFMongoConfig::Instance( "mongo.setting" );
    ////////////////////////////////////////////////////////////////////////////
}

#endif
