#ifndef __KF_PLATFORM_CONFIG_H__
#define __KF_PLATFORM_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
#define __TOTAL_SERVER_LIST__ 1	// 所有服务器列表
#define __SELECT_SERVER_DATA__ 2	// 直接选择登录

    class KFServerListType
    {
    public:
        KFServerListType()
        {
            _min_flag = 0;
            _max_flag = 0;
            _list_type = 0;
        }
    public:
        uint32 _min_flag;
        uint32 _max_flag;
        uint32 _list_type;
    };


    class KFPlatformConfig : public KFConfig, public KFSingleton< KFPlatformConfig >
    {
    public:
        KFPlatformConfig();
        ~KFPlatformConfig();

        bool LoadConfig( const char* file );

        uint32 GetServerListType( uint32 appflag );

    public:
        // token保存时间( 秒 );
        uint32 _token_expire_time;

        // 是否需要激活
        bool _is_open_activation;

        // 默认列表类型
        uint32 _default_server_list_type;

        // 服务器列表类型
        std::vector< KFServerListType > _server_list_type;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_platform_config = KFPlatformConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}


#endif