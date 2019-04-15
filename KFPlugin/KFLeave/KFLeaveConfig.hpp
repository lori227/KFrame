#ifndef __KF_LEAVE_CONFIG_H__
#define __KF_LEAVE_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFLeaveSetting
    {
    public:
        uint32 _note_id{ 0 };		// 服务器noteid
        std::string _lua_file;		// lua脚本
        std::string _lua_function;	// lua函数
    };
    ////////////////////////////////////////////////////////////////////////////////////

    class KFLeaveConfig : public KFConfig, public KFSingleton< KFLeaveConfig >
    {
    public:
        KFLeaveConfig() = default;
        ~KFLeaveConfig() = default;

        bool LoadConfig();

    public:
        std::vector< KFLeaveSetting > _kf_leave_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_leave_config = KFLeaveConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif