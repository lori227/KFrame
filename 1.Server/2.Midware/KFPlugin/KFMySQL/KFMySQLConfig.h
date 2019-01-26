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
        bool LoadConfig();

        // 查找配置
        const KFMySQLSetting* FindSetting( const std::string& module, uint32 logicid ) const;

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFMySQLSetting > _mysql_setting;
    };
}

////////////////////////////////////////////////////////////////////////////
static auto* _kf_mysql_config = KFrame::KFMySQLConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
