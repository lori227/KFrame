#ifndef __KF_SIGNIN_CONFIG_H__
#define __KF_SIGNIN_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    enum KFSignInEnum
    {
        SevenDay = 1,		// 7天签到
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFSignInSetting
    {
    public:
        uint32 _day;			// 签到天数
        KFElements _reward;		// 奖励
        uint32 _probability;	// 概率( 万分比 )
        KFElements _extend;		// 额外的奖励
    };

    ////////////////////////////////////////////////////////////////////////////////////

    class KFSignInConfig : public KFConfig, public KFSingleton< KFSignInConfig >
    {
    public:
        KFSignInConfig() = default;
        ~KFSignInConfig();

        bool LoadConfig( const std::string& file );

        const KFSignInSetting* FindSignInSetting( uint32 type, uint32 day ) const;

    public:
        // 称号列表
        typedef std::pair< uint32, uint32 > SignInKey;
        KFMap< SignInKey, const SignInKey&, KFSignInSetting > _signin_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_signin_config = KFSignInConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif