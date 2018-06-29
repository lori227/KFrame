#include "KFChatConfig.h"


namespace KFrame
{
    KFChatConfig::KFChatConfig()
    {
    }

    KFChatConfig::~KFChatConfig()
    {
    }

    bool KFChatConfig::LoadConfig( const char* file )
    {
        _kf_chat_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFChatSetting );

                kfsetting->_chat_type = xmlnode.GetUInt32( "ChatType" );
                kfsetting->_chat_interval = xmlnode.GetUInt32( "ChatInterval" );
                kfsetting->_chat_byte_length = xmlnode.GetUInt32( "ChatByteLength" );
                _kf_chat_setting.Insert( kfsetting->_chat_type, kfsetting );

                xmlnode.NextNode();
            }
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    const KFChatSetting* KFChatConfig::FindChatSetting( uint32 chattype ) const
    {
        return _kf_chat_setting.Find( chattype );
    }
}
