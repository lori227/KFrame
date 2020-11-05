#ifndef __KF_MYSQL_CONFIG_H__
#define __KF_MYSQL_CONFIG_H__

#include "KFrame.h"
#include "KFConfig.h"

namespace KFrame
{
    class KFMySQLSetting : public KFIntSetting
    {
    public:
        std::string _ip;
        uint32 _port = 0;
        std::string _user;
        std::string _password;
        std::string _database;
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
        KFMySQLSetting* _kf_seting = nullptr;
    };


    class KFMySQLType
    {
    public:
        // 查询list
        KFMySQLList* FindMySQLList( uint32 type );

        // 添加list
        KFMySQLList* AddMySQLList( uint32 type );

    public:
        uint32 _id = 0;

        // 列表
        KFHashMap< uint32, uint32, KFMySQLList > _mysql_list;
    };

    class KFMySQLConfig : public KFConfig, public KFInstance< KFMySQLConfig >
    {
    public:
        KFMySQLConfig()
        {
            _file_name = "mysql";
        }

        // 加载配置文件
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

        // 查找配置
        KFMySQLType* FindMySQLType( const std::string& module, uint32 logicid );

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFMySQLType > _mysql_type;
    };
}

#endif
