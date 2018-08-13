#ifndef __KF_MYSQL_H__
#define __KF_MYSQL_H__

#include "KFrame.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFMySQL
    {
    public:
        KFMySQL();
        virtual ~KFMySQL();

        // 初始化
        bool InitMySQL( uint32 id, const std::string& user, const std::string& password, const std::string& database, const std::string& ip, uint32 port );

        // 是否连接
        bool IsConnected();

    protected:
        Poco::Data::Session* _session;

    private:
        std::string _connect_data;
    };
}


#endif