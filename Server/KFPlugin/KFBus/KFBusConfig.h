#ifndef __KF_BUS_CONFIG_H__
#define __KF_BUS_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFConnection
    {
    public:
        KFConnection()
        {
        }

    public:
        std::string _app_name;
        std::string _app_type;
        std::string _app_id;

        std::string _connect_name;
        std::string _connect_type;
        std::string _connect_id;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFBusConfig : public KFConfig, public KFSingleton< KFBusConfig >
    {
    public:

        bool LoadConfig();

        // 查找连接
        void FindConnection( const std::string& appname, const std::string& apptype, const std::string& appid, std::set< KFConnection* >& outlist );

        // 判断是否是连接
        bool IsValidConnection( const std::string& appname, const std::string& apptype, const std::string& appid,
                                const std::string& connectname, const std::string& connecttype, const std::string& connectid );
    public:
        // 连接信息
        std::vector< KFConnection > _bus_connection;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_bus_config = KFBusConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif