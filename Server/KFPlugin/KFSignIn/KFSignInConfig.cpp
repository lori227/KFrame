#include "KFSignInConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFSignInConfig::~KFSignInConfig()
    {
        _signin_setting.Clear();
    }

    void KFSignInConfig::AddSignInSetting( uint32 type, uint32 day, KFSignInSetting* kfstting )
    {
        SignInKey key( type, day );
        _signin_setting.Insert( key, kfstting );
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

            auto kfsetting = __KF_CREATE__( KFSignInSetting );
            kfsetting->_day = xmlnode.GetUInt32( "Day" );
            kfsetting->_probability = xmlnode.GetUInt32( "Probability" );

            auto strreward = xmlnode.GetString( "Reward" );
            kfsetting->_reward.ParseFromString( strreward, __FUNC_LINE__ );

            auto strextend = xmlnode.GetString( "ExtendReward" );
            kfsetting->_extend.ParseFromString( strextend, __FUNC_LINE__ );

            AddSignInSetting( type, kfsetting->_day, kfsetting );
            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}