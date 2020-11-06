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
    KFChannelSetting* KFChannelConfig::CreateSetting( KFXmlNode& xmlnode )
    {
        auto service = xmlnode.ReadUInt32( "Service", true, _invalid_int );
        auto ok = KFGlobal::Instance()->CheckChannelService( _invalid_int, service );
        if ( !ok )
        {
            return nullptr;
        }

        return KFConfigT< KFChannelSetting >::CreateSetting( xmlnode );
    }

    void KFChannelConfig::ReadSetting( KFXmlNode& xmlnode, KFChannelSetting* kfsetting )
    {
        kfsetting->_login_url = xmlnode.ReadString( "LoginUrl", true );
        kfsetting->_pay_url = xmlnode.ReadString( "PayUrl", true );
        kfsetting->_app_id = xmlnode.ReadString( "AppId", true );
        kfsetting->_app_key = xmlnode.ReadString( "AppKey", true );
        kfsetting->_debug_open = xmlnode.ReadBoolen( "Debug", true );
        kfsetting->_release_open = xmlnode.ReadBoolen( "Release", true );
        kfsetting->_support_list = xmlnode.ReadUInt32Set( "Support", true );
    }
}