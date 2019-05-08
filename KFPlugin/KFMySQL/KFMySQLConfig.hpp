#ifndef __KF_MYSQL_CONFIG_H__
#define __KF_MYSQL_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFMySQLSetting
    {
    public:
        uint32 _id;
        std::string _ip;
        uint32 _port;
        std::string _user;
        std::string _password;
        std::string _database;
        std::string _name;
        uint32 _type;
    };

    class KFMySQLList
    {
    public:
        // 清空
        void Reset();

        const KFMySQLSetting* FindSetting();
        void AddSetting( KFMySQLSetting& kfsetting );

    private:
        std::vector< KFMySQLSetting > _mysql_list;

        // 返回的连接设置
        KFMySQLSetting* _kfseting;
    };


    class KFMySQLType
    {
    public:
        // 查询redislist
        KFMySQLList* FindMySQLList( uint32 type );

        // 添加redislist
        KFMySQLList* AddMySQLList( uint32 type );

    public:
        uint32 _id;

        // Redis列表
        KFHashMap< uint32, uint32, KFMySQLList > _mysql_list;
    };

    class KFMySQLConfig : public KFConfig, public KFSingleton< KFMySQLConfig >
    {
    public:

        // 加载配置文件
        bool LoadConfig();

        // 查找配置
        KFMySQLType* FindMySQLType( const std::string& module, uint32 logicid );

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFMySQLType > _mysql_type;
    };

    ////////////////////////////////////////////////////////////////////////////
    static auto* _kf_mysql_config = KFrame::KFMySQLConfig::Instance();
    ////////////////////////////////////////////////////////////////////////////
}

#endif
