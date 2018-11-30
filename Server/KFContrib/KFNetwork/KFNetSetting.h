#ifndef __NET_SETTING_H__
#define __NET_SETTING_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFNetSetting
    {
    public:
        KFNetSetting() : _id( 0 ), _port( 0 ), _timeout( 20 ), _max_connect( 20000 ), _connect_time( 1000 )
        {

        }

    public:
        uint64 _id;				// id
        std::string _type;		// 类型
        std::string _name;		// 名称
        std::string _ip;		// ip地址
        uint32 _port;			// 端口
        std::string _password;	// 密码
        std::string _user;		// 用户名
        uint32 _timeout;		// 超时时间
        uint32 _max_connect;	// 最大连接数
        uint32 _connect_time;	// 连接定时器

    };
}


#endif