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
            _app_id = 0;
            _connect_id = 0;
        }

    public:
        std::string _app_name;
        std::string _app_type;
        uint32 _app_id;

        std::string _connect_name;
        std::string _connect_type;
        uint32 _connect_id;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFBusConfig : public KFConfig, public KFSingleton< KFBusConfig >
    {
    public:

        bool LoadConfig( const char* file );

        // 查找连接
        void FindConnection( const std::string& appname, const std::string& apptype, uint32 appid, std::set< KFConnection* >& outlist );

        // 判断是否是连接
        bool IsValidConnection( const std::string& appname, const std::string& apptype, uint32 appid,
                                const std::string& connectname, const std::string& connecttype, uint32 connectid );

    protected:
        bool IsSame( const std::string& appname, const std::string& apptype, uint32 appid,
                     const std::string& connectname, const std::string& connecttype, uint32 connectid );

    public:
        // 连接信息
        std::vector< KFConnection > _bus_connection;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_bus_config = KFBusConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif