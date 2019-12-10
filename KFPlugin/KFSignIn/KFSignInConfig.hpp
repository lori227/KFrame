#ifndef __KF_SIGNIN_CONFIG_H__
#define __KF_SIGNIN_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    enum KFSignInEnum
    {
        SevenDay = 1,		// 7天签到
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFSignInSetting : public KFIntSetting
    {
    public:
        // 签到类型
        uint32 _type = 0u;

        // 签到天数
        uint32 _day = 0u;

        // 奖励
        KFElements _reward;

        // 概率( 万分比 )
        uint32 _probability = 0u;

        // 额外的奖励
        KFElements _extend;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFSignInConfig : public KFConfigT< KFSignInSetting >, public KFInstance< KFSignInConfig >
    {
    public:
        KFSignInConfig()
        {
            _file_name = "signin";
        }

    protected:
        // 赌球配置
        virtual void ReadSetting( KFNode& xmlnode, KFSignInSetting* kfsetting );
    };
}

#endif