#ifndef __KF_ROBOT_CONFIG_H__
#define __KF_ROBOT_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFRobotConfig : public KFConfig, public KFSingleton< KFRobotConfig >
    {
    public:
        // 读取配置
        bool LoadConfig();

    public:
        // 认证服务器地址
        std::string _auth_address;

        // 机器人总数
        uint32 _total_robot_count = 1u;

        // 登录的间隔时间
        uint32 _login_interval_time = 1000;

        // 开启属性更新log
        bool _open_data_log = false;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_robot_config = KFRobotConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif