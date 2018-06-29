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
    };

    class KFMySQLConfig : public KFConfig, public KFSingleton< KFMySQLConfig >
    {
    public:

        // 加载配置文件
        bool LoadConfig( const char* file );

        const KFMySQLSetting* FindSetting( uint32 id ) const;
        const KFMySQLSetting* FindSetting( const std::string& field, uint32 logicid ) const;

    protected:
        uint32 FindLogicMySQLId( const std::string& filed, uint32 logicid ) const;

    public:
        // 数据库连接
        KFMap< uint32, uint32, KFMySQLSetting > _mysql_setting;

        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > LogicKey;
        std::map< LogicKey, uint32 > _logic_mysql_map;
    };
}

////////////////////////////////////////////////////////////////////////////
static auto* _kf_mysql_config = KFrame::KFMySQLConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
