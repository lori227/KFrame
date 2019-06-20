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

        return KFIntConfigT< KFChannelSetting >::CreateSetting( xmlnode );
    }

    void KFChannelConfig::ReadSetting( KFNode& xmlnode, KFChannelSetting* kfsetting )
    {
        kfsetting->_login_url = xmlnode.GetString( "LoginUrl" );
        kfsetting->_pay_url = xmlnode.GetString( "PayUrl" );
        kfsetting->_app_id = xmlnode.GetString( "AppId" );
        kfsetting->_app_key = xmlnode.GetString( "AppKey" );
        kfsetting->_debug_open = xmlnode.GetBoolen( "Debug" );
        kfsetting->_release_open = xmlnode.GetBoolen( "Release" );

        auto stropen = xmlnode.GetString( "Support" );
        kfsetting->_support_list = KFUtility::SplitSet< std::set< uint32 > >( stropen, DEFAULT_SPLIT_STRING );
    }
}