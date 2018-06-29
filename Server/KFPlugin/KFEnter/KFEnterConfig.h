#ifndef __KF_ENTER_CONFIG_H__
#define __KF_ENTER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFEnterSetting
    {
    public:
        uint32 _note_id;	// 服务器noteid
        std::string _lua_file;	// lua脚本
        std::string _lua_function;	// lua函数
    };
    ////////////////////////////////////////////////////////////////////////////////////

    class KFEnterConfig : public KFConfig, public KFSingleton< KFEnterConfig >
    {
    public:
        KFEnterConfig();
        ~KFEnterConfig();

        bool LoadConfig( const char* file );

    public:
        std::vector< KFEnterSetting > _kf_enter_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_enter_config = KFEnterConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif