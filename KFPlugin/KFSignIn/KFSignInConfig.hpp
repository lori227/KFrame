#ifndef __KF_SIGNIN_CONFIG_H__
#define __KF_SIGNIN_CONFIG_H__

#include "KFCore/KFElement.h"

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
    class KFSignInConfig : public KFIntConfigT< KFSignInSetting >, public KFSingleton< KFSignInConfig >
    {
    public:
        KFSignInConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFSignInSetting >( file, isclear )
        {
        }

    protected:
        // 赌球配置
        void ReadSetting( KFNode& xmlnode, KFSignInSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_signin_config = KFSignInConfig::Instance( "signin.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif