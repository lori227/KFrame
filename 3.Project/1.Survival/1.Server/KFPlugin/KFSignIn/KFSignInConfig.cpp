#include "KFSignInConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFSignInConfig::~KFSignInConfig()
    {
        _signin_setting.Clear();
    }

    const KFSignInSetting* KFSignInConfig::FindSignInSetting( uint32 type, uint32 day ) const
    {
        SignInKey key( type, day );
        return _signin_setting.Find( key );
    }

    bool KFSignInConfig::LoadConfig()
    {
        _signin_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto type = xmlnode.GetUInt32( "Type" );
            auto day = xmlnode.GetUInt32( "Day" );

            SignInKey signkey( type, day );
            auto kfsetting = _signin_setting.Create( signkey );

            kfsetting->_day = day;
            kfsetting->_probability = xmlnode.GetUInt32( "Probability" );

            auto strreward = xmlnode.GetString( "Reward" );
            kfsetting->_reward.Parse( strreward, __FUNC_LINE__ );

            auto strextend = xmlnode.GetString( "ExtendReward" );
            kfsetting->_extend.Parse( strextend, __FUNC_LINE__ );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}