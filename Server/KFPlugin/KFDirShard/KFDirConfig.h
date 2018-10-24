#ifndef __KF_DIR_CONFIG_H__
#define __KF_DIR_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
#define __TOTAL_SERVER_LIST__ 1		// 所有服务器列表
#define __SELECT_SERVER_DATA__ 2	// 直接选择登录

    class KFDirConfig : public KFConfig, public KFSingleton< KFDirConfig >
    {
    public:
        KFDirConfig();
        ~KFDirConfig() = default;

        bool LoadConfig();

        // 获得服务器类型
        uint32 GetDirListType( uint32 appchannel );

    public:
        // 默认列表类型
        uint32 _default_dir_list_type;

        // 服务器列表类型
        std::map< uint32, uint32 > _dir_list_type;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_dir_config = KFDirConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}


#endif