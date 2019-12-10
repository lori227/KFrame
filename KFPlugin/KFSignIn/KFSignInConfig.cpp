#include "KFSignInConfig.hpp"

namespace KFrame
{
    void KFSignInConfig::ReadSetting( KFNode& xmlnode, KFSignInSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_day = xmlnode.GetUInt32( "Day" );
        kfsetting->_probability = xmlnode.GetUInt32( "Probability" );

        auto strreward = xmlnode.GetString( "Reward" );
        kfsetting->_reward.Parse( strreward, __FUNC_LINE__ );

        auto strextend = xmlnode.GetString( "ExtendReward" );
        kfsetting->_extend.Parse( strextend, __FUNC_LINE__ );
    }
}