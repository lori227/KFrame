#include "KFChannelConfig.hpp"

namespace KFrame
{
    bool KFChannelSetting::IsOpen() const
    {
#ifdef __KF_DEBUG__
        return _debug_open;
#else
        return _release_open;
#endif
    }

    bool KFChannelSetting::IsSupport( uint32 channel ) const
    {
        if ( _support_list.empty() )
        {
            return false;
        }

        return _support_list.find( channel ) != _support_list.end();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFChannelSetting* KFChannelConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true, _invalid_int );
        auto ok = KFGlobal::Instance()->CheckChannelService( _invalid_int, service );
        if ( !ok )
        {
            return nullptr;
        }

        return KFConfigT< KFChannelSetting >::CreateSetting( xmlnode );
    }

    void KFChannelConfig::ReadSetting( KFNode& xmlnode, KFChannelSetting* kfsetting )
    {
        kfsetting->_login_url = xmlnode.GetString( "LoginUrl", true );
        kfsetting->_pay_url = xmlnode.GetString( "PayUrl", true );
        kfsetting->_app_id = xmlnode.GetString( "AppId", true );
        kfsetting->_app_key = xmlnode.GetString( "AppKey", true );
        kfsetting->_debug_open = xmlnode.GetBoolen( "Debug", true );
        kfsetting->_release_open = xmlnode.GetBoolen( "Release", true );

        auto stropen = xmlnode.GetString( "Support", true );
        KFUtility::SplitSet( kfsetting->_support_list, stropen, __SPLIT_STRING__ );
    }
}