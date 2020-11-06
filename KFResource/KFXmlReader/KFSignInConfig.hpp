#ifndef __KF_SIGNIN_CONFIG_H__
#define __KF_SIGNIN_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

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


        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;

                KFElementConfig::Instance()->ParseElement( kfsetting->_reward, __FILE__, __LINE__ );
                KFElementConfig::Instance()->ParseElement( kfsetting->_extend, __FILE__, __LINE__ );
            }
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFSignInSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.ReadUInt32( "Type" );
            kfsetting->_day = xmlnode.ReadUInt32( "Day" );
            kfsetting->_probability = xmlnode.ReadUInt32( "Probability" );

            kfsetting->_reward._str_parse = xmlnode.ReadString( "Reward" );
            kfsetting->_extend._str_parse = xmlnode.ReadString( "ExtendReward" );
        }
    };
}

#endif