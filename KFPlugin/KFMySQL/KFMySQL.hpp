#ifndef __KF_MYSQL_H__
#define __KF_MYSQL_H__

#include "KFrame.h"
#include "KFMySQLConfig.hpp"
#include "Poco/Data/Session.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"

namespace KFrame
{
    using namespace Poco::Data;
    class KFMySQL
    {
    public:
        KFMySQL();
        virtual ~KFMySQL();

        // 初始化
        virtual void InitMySQL( const KFMySQLSetting* kfsetting );

        // 关闭
        virtual void ShutDown();

        // 是否连接
        bool IsConnected();

    protected:
        Session* _session;
        std::string _connect_data;
    };
}


#endif